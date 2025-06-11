# Build Instructions for `lib/snowball`

## Option 1: Build for Unix/Linux
```bash
make clean
make
```

## Option 2: Cross-Compile for Windows (using MinGW)
```bash
sudo dnf install mingw64-gcc mingw64-gcc-c++
make clean
make CC=x86_64-w64-mingw32-gcc EXEEXT=.exe
```

---

# Compile Test Files

## compare.cpp

```bash
g++ -I../lib/snowball/include -L../lib/snowball -o c compare.cpp -lstemmer
```

```bash
clang++ -I../lib/snowball/include -L../lib/snowball -o c compare.cpp -lstemmer
```

```bash
x86_64-w64-mingw32-g++ -static -static-libgcc -static-libstdc++ \
  -I../lib/snowball/include -o c.exe compare.cpp ../lib/snowball/libstemmer.a
```

---

## test.cpp

```bash
g++ -I../lib/snowball/include -L../lib/snowball -o t test.cpp -lstemmer
```

```bash
clang++ -I../lib/snowball/include -L../lib/snowball -o t test.cpp -lstemmer
```

```bash
x86_64-w64-mingw32-g++ -static -static-libgcc -static-libstdc++ \
  -I../lib/snowball/include -o t.exe test.cpp ../lib/snowball/libstemmer.a
```

---

## perf.cpp

```bash
g++ -I../lib/snowball/include -L../lib/snowball -o p perf.cpp -lstemmer
```

```bash
clang++ -I../lib/snowball/include -L../lib/snowball -o p perf.cpp -lstemmer
```

```bash
x86_64-w64-mingw32-g++ -static -static-libgcc -static-libstdc++ \
  -I../lib/snowball/include -o p.exe perf.cpp ../lib/snowball/libstemmer.a
```

---

# Wordlists for Testing

- **English**: https://github.com/dwyl/english-words/tree/master  
- **Russian**: https://github.com/danakt/russian-words  
- **French**: https://github.com/Taknok/French-Wordlist/tree/master
