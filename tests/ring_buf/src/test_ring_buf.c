/*
 * Ring Buffer Module - Homework Test Skeleton
 *
 * test_fresh_state is provided as a worked example. Fill in the remaining
 * 7 ZTEST bodies according to TEST_SPEC.md. Stubs call ztest_test_skip()
 * so the binary builds and runs cleanly before each test is implemented.
 *
 * Run:
 *   west twister -T tests/ring_buf -p native_sim
 */

#include <zephyr/ztest.h>
#include <errno.h>

#include "ring_buf.h"

/*
 * Shared before hook: every suite reinitialises the ring buffer with a
 * capacity of 4 so tests start from a clean, known state. Capacity 4 is
 * enough to exercise FIFO order (push 1, 2, 3) and overflow (full at 4).
 */
static void before(void *f)
{
	ARG_UNUSED(f);
	rb_init(4);
}

/*
 * ============================================================================
 * Test Suite: ring_buf_init
 *
 * Initial state and re-initialization behaviour.
 * ============================================================================
 */
ZTEST_SUITE(ring_buf_init, NULL, NULL, before, NULL, NULL);

/* PROVIDED — study this test before writing the rest. */
ZTEST(ring_buf_init, test_fresh_state)
{
	zassert_true(rb_is_empty(), "Fresh buffer must be empty");
	zassert_equal(rb_count(), 0, "Fresh buffer count must be 0");
}

ZTEST(ring_buf_init, test_reinit_clears_state)
{
	// preconditions:
	zassert_ok(rb_push(99), "push must succeed");
	zassert_equal(rb_count(), 1, "Count must be 1 after one push");
	
	rb_init(4); // reinitialised

	// Verify #2: 
	// - after the second rb_init(4), rb_is_empty() == true and rb_count() == 0 
	// (the previous push is fully discarded).
	zassert_true(rb_is_empty(), "Fresh buffer must be empty");
	zassert_equal(rb_count(), 0, "Fresh buffer count must be 0");
}

/*
 * ============================================================================
 * Test Suite: ring_buf_push_pop
 *
 * Single push/pop round-trip, FIFO order, full error path.
 * ============================================================================
 */
ZTEST_SUITE(ring_buf_push_pop, NULL, NULL, before, NULL, NULL);

ZTEST(ring_buf_push_pop, test_single_push_pop)
{
	int value = 0;

	// Verify #1: 
	// push(42) == 0; 
	// pop(&v) == 0; v == 42; 
	// after the pop rb_is_empty() == true.
	zassert_ok(rb_push(42), "push must succeed");
	
	zassert_ok(rb_pop(&value), "pop must succeed");
	zassert_equal(value, 42, "Value must be 42");

	zassert_true(rb_is_empty(), "Fresh buffer must be empty");
}

ZTEST(ring_buf_push_pop, test_fifo_order)
{

	int value = 0;
	
	// Verify #2: 
	// all three push(...) calls return 0; 
	// the three pop(&v) calls yield 1, then 2, then 3 (use zassert_equal per pop). 
	// After the third pop rb_is_empty() == true.
	zassert_ok(rb_push(1), "push(1) must succeed");
	zassert_ok(rb_push(2), "push(2) must succeed");
	zassert_ok(rb_push(3), "push(3) must succeed");

	zassert_ok(rb_pop(&value), "pop 1 must succeed");
	zassert_equal(value, 1, "Value must be 1");
	zassert_ok(rb_pop(&value), "pop 2 must succeed");
	zassert_equal(value, 2, "Value must be 2");
	zassert_ok(rb_pop(&value), "pop 3 must succeed");
	zassert_equal(value, 3, "Value must be 3");

	zassert_true(rb_is_empty(), "Fresh buffer must be empty");
}

ZTEST(ring_buf_push_pop, test_push_full_returns_enospc)
{
	// preconditions
	zassert_ok(rb_push(1), "push(1) must succeed");
	zassert_ok(rb_push(2), "push(2) must succeed");
	zassert_ok(rb_push(3), "push(3) must succeed");
	zassert_ok(rb_push(4), "push(4) must succeed");

	// Verify #3: 
	// rb_is_full() == true; 
	// the fifth push(99) returns exactly -ENOSPC; 
	// rb_count() == 4 (rejected push did not consume a slot).
	zassert_true(rb_is_full(), "Buffer must be full");

	zassert_equal(rb_push(99), -ENOSPC, "push must return -ENOSPC");

	zassert_equal(rb_count(), 4, "Buffer count must be 4");
}

/*
 * ============================================================================
 * Test Suite: ring_buf_boundaries
 *
 * Peek semantics and NULL-pointer boundary conditions.
 * ============================================================================
 */
ZTEST_SUITE(ring_buf_boundaries, NULL, NULL, before, NULL, NULL);

ZTEST(ring_buf_boundaries, test_peek_does_not_consume)
{
	int value = 0;

	// precondition
	zassert_ok(rb_push(7), "push must succeed");

	// Verify #1: 
	// the first and second peek(&v) both return 0 and write 7 into v. 
	// After the two peeks rb_count() == 1 (peek must not consume).
	zassert_ok(rb_peek(&value), "First peek must succeed");
	zassert_equal(value, 7, "Value must be 7");
	value = 0; // reinit
	zassert_ok(rb_peek(&value), "Second peek must succeed");
	zassert_equal(value, 7, "Value must be 7");	

	zassert_equal(rb_count(), 1, "Buffer count must be 1");

}

ZTEST(ring_buf_boundaries, test_pop_null_returns_einval)
{
	// Verify #2: 
	// rb_pop(NULL) returns exactly -EINVAL.
	zassert_equal(rb_pop(NULL), -EINVAL, "pop(NULL) must return -EINVAL");

}

ZTEST(ring_buf_boundaries, test_is_full_after_fill)
{

	// Verify #3: 
	// all four pushes return 0; 
	// rb_is_full() == true; 
	// rb_count() == 4.
	zassert_ok(rb_push(1), "push(1) must succeed");
	zassert_ok(rb_push(2), "push(2) must succeed");
	zassert_ok(rb_push(3), "push(3) must succeed");
	zassert_ok(rb_push(4), "push(4) must succeed");

	zassert_true(rb_is_full(), "Buffer must be full");

	zassert_equal(rb_count(), 4, "Buffer count must be 4");
}
