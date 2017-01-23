gvar := "mark";

f := function(a)
    local x;
    gvar := "start";
    if a > 0 then
        gvar := "inif";
    fi;
    gvar := "ifend";
end;
