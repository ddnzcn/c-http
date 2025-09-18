# Http server in C

## A litte warning
This project is born out of boredom. No memory safety is 
guaranteed, I don't even check malloc calls for the most part.

## Why C?
Because I want to deep dive into systems programming and most of my career I've
been building web/mobile apps. Even though I've worked with Go as in some side
projects, I wanted to go one level lower.

# How to build?
I've included a simple `makefile` just run

```
make
```

and start the application with 

```
./bin/http_server
```

it doesn't even take arguments.

# TODO's
- [ ] Learn more about memory safe ways to write C
- [ ] Parse arguments to pass a configs (with defaults)
- [ ] Making a generic hashmap for Header parsing
- [ ] Parsing path params
- [ ] Parsing query params
- [ ] Building a generic matcher
- [ ] Proper multi-therading instead of using poll
