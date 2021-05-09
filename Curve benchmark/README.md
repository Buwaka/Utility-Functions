``` ini

BenchmarkDotNet=v0.12.1, OS=Windows 10.0.19042
Intel Core i5-8300H CPU 2.30GHz (Coffee Lake), 1 CPU, 8 logical and 4 physical cores
  [Host]     : .NET Framework 4.8 (4.8.4341.0), X86 LegacyJIT  [AttachedDebugger]
  DefaultJob : .NET Framework 4.8 (4.8.4341.0), X86 LegacyJIT


```
|     Method |     Mean |    Error |   StdDev |
|----------- |---------:|---------:|---------:|
| CheapCurve | 10.60 ns | 0.462 ns | 0.432 ns |
|   CosCurve | 34.50 ns | 0.414 ns | 0.367 ns |
