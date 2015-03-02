#import <dispatch/dispatch.h>
#import <Foundation/NSError.h>
#import <Foundation/NSArray.h>
#import <PromiseKit/fwd.h>

/**
 A promise represents the future value of a task.

 To obtain the value of a promise we call `then`.

 Promises are chainable: `then` returns a promise, you can call `then` on that promise, which then  returns a promise, with which you can call `then`, et cetera.

 @see [PromiseKit `then` Guide](http://promisekit.org/then/)
 @see [PromiseKit Chaining Guide](http://promisekit.org/chaining/)
*/
@interface PMKPromise : NSObject

/**
 Executes the provided block when *this* promise is resolved, the value of this promise is passed to the provided block as its parameter.

 @param block The block that is executed when this promise is resolved.

    [NSURLConnection GET:url].then(^(NSData *data){
        // do something with data
    });

 @return A new promise that is resolved with the value returned from the provided block. For example:

    [NSURLConnection GET:url].then(^(NSData *data){
        return data.length;
    }).then(^(NSNumber *number){
        //…
    });

 @warning *Important* The block passed to `then` may take zero, one, two or three arguments, and return an object or return nothing. This flexibility is why the method signature for then is `id`, which means you will not get completion for the block parameter, and must type it yourself. It is safe to type any block syntax here, so to start with try just: `^{}`.

 @warning *Important* If an exception is thrown inside your block, or you return an `NSError` object the next `Promise` will be rejected. @see `catch` for documentation on error handling.

 @warning *Important* `then` is always executed on the main queue.

 @see thenOn
 @see thenInBackground
*/
- (PMKPromise *(^)(id))then;

/**
 The provided block is executed in the background on a default global queue.
 */
- (PMKPromise *(^)(id))thenInBackground;

/**
 The provided block always runs on the main queue.
*/
- (PMKPromise *(^)(id))catch;

/**
 The provided block always runs on the main queue.
*/
- (PMKPromise *(^)(void(^)(void)))finally;

/**
 The provided block is executed on the dispatch queue of your choice.
*/
- (PMKPromise *(^)(dispatch_queue_t, id))thenOn;

/**
 The provided block is executed on the dispatch queue of your choice.
*/
- (PMKPromise *(^)(dispatch_queue_t, id))catchOn;

/**
 The provided block is executed on the dispatch queue of your choice.
*/
- (PMKPromise *(^)(dispatch_queue_t, void(^)(void)))finallyOn;

/**
 Creates a resolved promise.

 When developing your own promise systems, it is ocassionally useful to be able to return an already resolved promise.

 @param value The value with which to resolve this promise. Passing an `NSError` will cause the promise to be rejected, otherwise the promise will be fulfilled.

 @return A resolved promise.
*/
+ (PMKPromise *)promiseWithValue:(id)value;


/// @return `YES` if the promise has not yet resolved.
- (BOOL)pending;

/// @return `YES` if the promise has resolved (ie. is fulfilled or rejected) `NO` if it is pending.
- (BOOL)resolved;

/// @return `YES` if the promise is fulfilled, `NO` if it is rejected or pending.
- (BOOL)fulfilled;

/// @return `YES` if the promise is rejected, `NO` if it is fulfilled or pending.
- (BOOL)rejected;

/**
 The value of the asynchronous task this promise represents.

 A promise has `nil` value if the asynchronous task it represents has not
 finished. If the value is `nil` the promise is still `pending`.

 @returns If `resolved` the object that was used to resolve this promise,
 if `pending` nil.
*/
- (id)value;

/**
 Create a new promise.

 Use this method when wrapping asynchronous code that does *not* use
 promises so that this code can be used in promise chains.

 Don’t use this method if you already have promises! Instead, just
 return your promise.

 @param block The provided block is immediately executed, any exceptions that occur will be caught and cause the returned promise to be rejected.
   - @param fulfill fulfills the returned promise with the provided value
   - @param reject rejects the returned promise with the provided `NSError`

 Should you need to fulfill a promise but have no sensical value to use;
 your promise is a `void` promise: fulfill with `nil`.

 The block you pass is executed immediately on the calling thread.

 @return A new promise.

 @see http://promisekit.org/sealing-your-own-promises/
 @see http://promisekit.org/wrapping-delegation/
*/
+ (instancetype)new:(void(^)(PMKPromiseFulfiller fulfill, PMKPromiseRejecter reject))block;

@end



/**
 Use with `+new:`, or return from a `then` or `catch` handler to fulfill
 a promise with multiple arguments.

 Consumers of your Promise are not compelled to consume any arguments and
 in fact will often only consume the first parameter. Thus ensure the
 order of parameters is: from most-important to least-important.
 
 Currently PromiseKit limits you to THREE parameters to the manifold.
*/
#define PMKManifold(...) __PMKManifold(__VA_ARGS__, 3, 2, 1)
#define __PMKManifold(_1, _2, _3, N, ...) __PMKArrayWithCount(N, _1, _2, _3)
extern id __PMKArrayWithCount(NSUInteger, ...);



/**
 Executes the provided block on a background queue.

 dispatch_promise is a convenient way to start a promise chain where the first step

 @param block The block to be executed in the background. Returning an `NSError` will reject the promise, everything else (including void) fulfills the promise.

 @return A promise resolved with the provided block.

 @see dispatch_async
*/
PMKPromise *dispatch_promise(id block);

/**
 Executes the provided block on the specified queue.

 @see dispatch_promise
 @see dispatch_async
*/
PMKPromise *dispatch_promise_on(dispatch_queue_t q, id block);



/**
 Called by PromiseKit in the event of unhandled errors.
 The default handler NSLogs the error. Note, your handler is executed
 from an undefined queue, unless you manage thread-safe data, dispatch to
 a safe queue before doing anything else in your handler.
*/
extern void (^PMKUnhandledErrorHandler)(NSError *);
