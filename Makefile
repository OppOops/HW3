VMS = simple_jvm/jvm simple_dvm/dvm
DEX = tests/Benchmark/src/dhry.java tests/Benchmark/src/Benchmark.java

all: $(VMS)

simple_jvm/jvm:
	$(MAKE) -C simple_jvm

simple_dvm/dvm:
	$(MAKE) -C simple_dvm

dex_compile:
	$(MAKE) -C tests/old_Benchmark
	$(MAKE) -C tests/Benchmark


run: $(VMS) dex_compile
	simple_dvm/dvm tests/Benchmark/dhry.dex
run2: $(VMS) dex_compile
	simple_dvm/dvm tests/old_Benchmark/Benchmark.dex

check: $(VMS)
	simple_jvm/jvm tests/Foo1.class > output-jvm
	simple_dvm/dvm tests/Foo1.dex > output-dvm
	@diff -u output-jvm output-dvm || echo "ERROR: different results"

clean:
	$(MAKE) -C simple_jvm clean
	$(MAKE) -C simple_dvm clean
	$(RM) output-jvm output-dvm
