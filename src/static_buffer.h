/**
 * 
 * 
 * Defines macros to ease the creation of expandable static buffers
 * (that points to pointers)
 */
#ifndef __STATIC_BUFFER_H_
#define __STATIC_BUFFER_H_
/** Define the ASSERT macro, in case it still doesn't exists */
#  ifndef ASSERT
#    define ASSERT(stmt, err)
#  endif

/**
 * Create a expandable buffer of a given type
 * 
 * @param TYPE The type
 */
#define BUF_DEFINE(TYPE) \
    typedef struct { \
        TYPE **arr; \
        int len; \
        int used; \
    } TYPE##_static_buffer; \
    TYPE##_static_buffer _##TYPE##_buf

/**
 * Zero out a buffer
 * 
 * @param TYPE The type
 */
#define BUF_ZERO(TYPE) \
    do { \
        _##TYPE##_buf.arr = 0; \
        _##TYPE##_buf.len = 0; \
        _##TYPE##_buf.used = 0; \
    } while (0)

/**
 * Set the minimum size of a buffer, expanding it if necessary
 * 
 * @param TYPE The type
 * @param SIZE Minimum size for the buffer
 * @param ERR_CODE Error code set by ASSERT, if allocation failed
 * @param INI_FUNC Function to be called for newly instantiated stuff (e.g., if
 *                 type is an pointer and must be allocated). Make sure its only
 *                 parameter is a pointer to TYPE. Also, it must return an int
 *                 and, on success, 0
 */
#define BUF_SET_MIN_SIZE(TYPE, SIZE, ERR_CODE, INI_FUNC) \
    do { \
        if (_##TYPE##_buf.len < (SIZE)) { \
            int i = _##TYPE##_buf.len; \
            /* Alloc the new buffer */ \
            TYPE **tmp = (TYPE**)malloc(sizeof(TYPE*) * (SIZE)); \
            ASSERT(tmp, ERR_CODE); \
            /* Clean it up */ \
            memset(tmp, 0x0, sizeof(TYPE*) * (SIZE)); \
            /* Copy the old buffer and release its memory */ \
            if (_##TYPE##_buf.arr) { \
                memcpy(tmp, _##TYPE##_buf.arr, sizeof(TYPE*) * _##TYPE##_buf.len); \
                free(_##TYPE##_buf.arr); \
            } \
            /* Initialize every new node */ \
            while (i < (SIZE)) { \
                int ret; \
                ret = INI_FUNC(&tmp[i]); \
                ASSERT(ret == 0, ERR_CODE); \
                i++; \
            } \
            /* Update both the buffer and its size */ \
            _##TYPE##_buf.arr = tmp; \
            _##TYPE##_buf.len = SIZE; \
        } \
    } while (0)

/**
 * Alloc a instance of a type and check for errors
 * 
 * @param TYPE The type
 * @param REF Variable's name where the reference will be returned. Must be a
 *            double pointer to TYPE
 * @param ERR_CODE Error code set by ASSERT, if allocation failed
 */
#define BUF_ALOC_OBJ(TYPE, REF, ERR_CODE) \
    do { \
        /* Alloc the object */ \
        TYPE *tmp = (TYPE*)malloc(sizeof(TYPE)); \
        ASSERT(tmp, ERR_CODE); \
        *REF = tmp; \
    } while (0)

/**
 * Dealoc a variable and clean it
 * 
 * @param REF Variable that will be dealloc'ed. Must be a double pointer.
 */
#define BUF_DEALLOC_OBJ(REF) \
    do { \
        if (REF && *REF) { \
            free(*REF); \
            *REF = 0; \
        } \
    } while (0)

/**
 * Clean up all memory used by this type's buffer
 * 
 * @param TYPE The type
 * @param CLEAN_FUNC Function to be called per buffer object. Its only parameter
 *                   must be a pointer to TYPE
 */
#define BUF_CLEAN(TYPE, CLEAN_FUNC) \
    do { \
        /* Check that the array isn't NULL */ \
        if (_##TYPE##_buf.arr) { \
            int i = 0; \
            /* Dealloc every object on the buffer */\
            while (i < _##TYPE##_buf.len) { \
                CLEAN_FUNC(&(_##TYPE##_buf.arr[i])); \
                i++; \
            } \
            /* Release the buffer's memory */\
            free (_##TYPE##_buf.arr); \
            BUF_ZERO(TYPE); \
        } \
    } while (0)

/**
 * Get the reference to the next object (and expand the buffer as necessary)
 * 
 * @param TYPE The type
 * @param INC By how much should the buffer expand, if necessary
 * @param REF Variable's name where the reference will be returned. Must be a
 *            pointer to TYPE
 * @param ERR_CODE Error code set by ASSERT, if allocation failed
 * @param INI_FUNC Function to be called for newly instantiated stuff (e.g., if
 *                 type is an pointer and must be allocated). Make sure its only
 *                 parameter is a pointer to TYPE
 */
#define BUF_GET_NEXT_REF(TYPE, INC, REF, ERR_CODE, INI_FUNC) \
    do { \
        /* Check if the buffer must be expanded */ \
        if (_##TYPE##_buf.used >= _##TYPE##_buf.len) { \
            BUF_SET_MIN_SIZE(TYPE, _##TYPE##_buf.len + INC, ERR_CODE, INI_FUNC); \
        } \
        /* Get a valid new reference */ \
        REF = _##TYPE##_buf.arr[_##TYPE##_buf.used]; \
    } while (0)

/**
 * Validate a previous "BUF_GET_NEXT_REF" by increasing the amount of used items
 * 
 * @param TYPE The type
 */
#define BUF_PUSH(TYPE) \
    _##TYPE##_buf.used++

/**
 * Get how many objects are in use
 * 
 * @param TYPE The type
 */
#define BUF_GET_USED(TYPE) \
    _##TYPE##_buf.used

/**
 * Get a object from the buffer
 * 
 * @param TYPE The type
 */
#define BUF_GET_OBJECT(TYPE, NUM) \
    _##TYPE##_buf.arr[NUM]

/**
 * Reset the buffer so it restart
 * 
 * @param TYPE The type
 */
#define BUF_RESET(TYPE) \
    _##TYPE##_buf.used = 0

/**
 * Call something in every object int the buffer
 * 
 * @param TYPE The type
 * @param CALL_INI_ Initial portition of the calling function. It must expect a
 *                  following parameter of type TYPE
 * @param _CALL_END Final portition of the calling function. It must expect a
 *                  following parameter of type TYPE
 */
#define BUF_CALL_ALL(TYPE, CALL_INI_, _CALL_END) \
    do { \
        int i = 0; \
        while (i < _##TYPE##_buf.used) { \
            CALL_INI_##_##TYPE##_buf.arr[i]##_CALL_END ; \
            i++; \
        } \
    } while (0)

#endif

