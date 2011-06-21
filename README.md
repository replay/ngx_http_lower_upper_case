Nginx lower-/uppercase
-------

This module simply uppercases or lowercases a string and saves it into a new variable.

It knows only two directives: upper and lower

Example
===============

        set $var1 "hello";
        set $var2 "WORLD";

        upper $var3 "$var1 $var2"; // $var3 will be "HELLO WORLD"
        lower $var4 "$var1 $var2"; // $var4 will be "hello world"

