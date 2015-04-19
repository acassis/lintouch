-- this simple lintouch plugin scripted in Lua demonstrates how to use Lintouch
-- Lua API
--
print( "Hello world from Lua backend" )

print( "The following variables are defined" )
for k,v in pairs( variables ) do
    print( k, v )
end

texts = {
    "Tired of C?"; "Tired of C++?"; "Try Lua...";
    "See http://www.lua.org for more details" }

i = 1
while should_run() do
    set_real_value( variables.varSTR, texts[ i ] )
    i = i + 1
    if i > table.getn( texts ) then
        i = 1
    end
    sync_real_values()
    sleep( 3 * 1000 * 1000 )
end

-- vim: set ft=lua et ts=4 sw=4 tw=72:
