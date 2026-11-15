.PHONY: count test

tests = test_kover test_bounding_box test_describe test_draw \
        test_dynamic_allocation test_help test_memory test_quality \
        test_summarize test_validate

test:
	@for t in $(tests); do bats-core/bin/bats $$t.bats || exit 1; done

count:
	@for t in $(tests); do bats-core/bin/bats -c $$t.bats; done
