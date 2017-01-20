gvar := "mark";

f := function()
    local x;
    x := 1;
    gvar := "change";
    x := 2;
end;
