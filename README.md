OptionsParser
====================================================================
Parse complex options from the command line.

It also parses options from a rsourcefile in option value pair format. 

OptionsParser implements the traditional POSIX style character options ( -n ) as well as the newer GNU style long options ( --name ). 

You can also use a simpler long option version ( -name ) by asking to ignore the POSIX style options. 

OptionsParser supports the traditional UNIX resourcefile syntax of, any line starting with "#" is a comment and the value pairs use ":" as a delimiter. 

An option which expects a value is considered as an option value pair, while options without a value are considered flags. 

##Examples
```cpp
#include <iostream>

int main(){
    OptionsParser options;
    options.setOption("input");
    options.processCommandArgs(argc, argv);

    if(options.hasOptions()){
        if (options.getValue("input") != nullptr)
            std::cout << "... processing input ...\n"
    }
    else{
        std::cout << "No options provided.\n";
    }
}
```

Then use it like this.

```bash
    ./program_name --input path/to/file
```

See demo.cpp for more examples.

#####This project was originally a fork of [AnyOption](https://github.com/hackorama/AnyOption)
