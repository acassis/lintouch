-- this simple lintouch plugin scripted in Lua demonstrates how to use Lintouch
-- Lua API

while should_run() do

    -- lock requested values so that we can wait for them to change
    lock_requested_values()

    -- wait up to 500 miliseconds for new requested values, if they don't
    -- arrive, re-lock the varset and try again
    while not wait_for_new_requested_values( 1 * 1000 * 500 ) do
        unlock_requested_values()
        lock_requested_values()
    end

    -- loop all dirty requested values back
    for k,v in pairs( variables ) do
        if is_requested_value_dirty( v ) then
            set_real_value( v, get_requested_value( v ) )
        end
    end

   -- clean dirty flag on requested values
   sync_requested_values()
   unlock_requested_values()

   -- sync results back to the user
   sync_real_values()

end

-- vim: set ft=ada et ts=4 sw=4 tw=72:
