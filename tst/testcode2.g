gvar2 := "mark";

g := function(a)
    gvar2 := a;
end;

f := function()
    local x;
    g("A");
    g("B");
    g("C");
end;
