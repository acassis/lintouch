-- this simple lintouch plugin scripted in Lua demonstrates how to use Lintouch
-- Lua API

-- initialize to "known" values
for k,v in pairs( variables ) do
    rv = get_real_value( v )
    if type( rv ) == "boolean" then
        set_real_value( v, false )
    elseif type( rv ) == "number" then
        set_real_value( v, 0 )
    elseif type( rv ) == "string" then
        set_real_value( v, "0" )
    end
end

sync_real_values()

while should_run() do

    -- increment all real values by one
    for k,v in pairs( variables ) do
        rv = get_real_value( v )
        if type( rv ) == "boolean" then
            set_real_value( v, not rv )
        elseif type( rv ) == "number" then
            set_real_value( v, rv + 1 )
        elseif type( rv ) == "string" then
            set_real_value( v, tostring( tonumber( rv ) + 1 ) )
        end
    end

    -- sync results back to the user
    sync_real_values()

    -- sleep for one second
    sleep( 1 * 1000 * 1000 )

end

-- vim: set ft=lua et ts=4 sw=4 tw=72:
