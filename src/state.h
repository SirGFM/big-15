#ifndef __STATE_H__
#define __STATE_H__

/** Controls the currently executing state. */
struct stateHandler {
    /**
     * Configure a newly retrieved state. Should (usually) be called only
     * once for each instance.
     *
     * @param [in]self The stateHandler.
     * @return 0 on success, anything else on error.
     */
    int (*setup)(void *self);

    /**
     * Check whether the state is still running.
     *
     * @param [in]self The stateHandler.
     * @return A boolean indicating whether this state is running.
     */
    int (*isRunning)(void *self);

    /**
     * Handle event, update and draw the state.
     *
     * @param [in]self The stateHandler.
     */
    void (*update)(void *self);

    /**
     * Retrieve the next state.
     *
     * @param [in]self The stateHandler.
     * @return The next 'state'.
     */
    int (*nextState)(void *self);

    /**
     * Release all resources alloc'ed by this state. Should be called only
     * once for each instance.
     *
     * @param [in]self The stateHandler.
     */
    void (*release)(void *self);

    /** State to which we should return on 'pop()'. */
    struct stateHandler *parent;
};

/**
 * Stash the currently executing state, '*self', and replace it by 'tmp'.
 * The stashed state is stored in 'tmp->parent', so it may be recovered by
 * calling 'pop()'.
 *
 * The state should have just been alloc'ed, and this function will call
 * its 'setup()'.
 *
 * @param [in/out]self Holds the current state and is replaced by tmp.
 * @param [in]tmp New state (to take place over '*self').
 */
void push(struct stateHandler **self, struct stateHandler *tmp);

/**
 * Recover the stashed state, releasing (i.e., calls '(*self)->release()')
 * the pushed state in the process.
 *
 * @param [in/out]self Previously pushed state, to be replaced by its caller.
 */
void pop(struct stateHandler **self);

#endif /* __STATE_H__ */
