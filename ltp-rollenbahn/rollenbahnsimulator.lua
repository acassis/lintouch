-- The simulator for Rollehnbahn

-- refresh period (in seconds)
REFRESH = 0.1 * 1000 * 1000

-- animation speed [1, 2, 5, 10]
STEP = 2

-- after how many cycles to go crazy because of platform overrun
GOCRAZY_BEYOND = 1

-- after how many cycles to go crazy because of slow elevator movement
GOCRAZY_SLOW = 2

print( "Rollehnbahn Simulator started\n" )

-- reset all variables to the inital state
-- the part of the elevator system that carries the cargo now
-- #0 - upper, 1 - right, 2 - lower, 3 - left
cargo = 0

-- automatic or manual operation
auto = false

-- positions of the parts of the elevator
upper_pos, left_pos, lower_pos, right_pos = 0, 0, 0, 0

step = STEP

gocrazy_beyond, gocrazy_slow = 0, 0

upper_go = false
right_go_down = false
right_go_up = false
lower_go = false
left_go_down = false
left_go_up = false

slow = 0

-- the counter used for blinking the LEDs
blinkerror = 0
error_tooslow, error_overrun = false, false


function moveright( rght_pos, slw, stp )
    if rght_pos > 0 then
        if slw > 0 then
            if math.mod( slw , 4 ) == 0 then
                rght_pos = rght_pos - stp
            end
            slw = slw + 1
        else
            rght_pos = rght_pos - stp
        end
    end
    return rght_pos, slw
end

-- THIS IS THE MAIN LOOP
while should_run() do

    lock_requested_values()

    -- AUTOMATIC MODE
    if auto and get_requested_value( variables.reqmanual ) then
        auto = false
        print( "Switching to MANUAL mode" )
    elseif auto then
        if cargo == 0 then -- the upper conveyor
            if upper_pos < 100 then
                upper_pos = upper_pos + step
            elseif right_pos == 0 then
                cargo = 1
            else
                auto = false
                blinkerror = 0
                error_tooslow = true
            end

            if auto then
                right_pos, slow = moveright(right_pos, slow, step)
                if left_pos > 0 then
                    left_pos = left_pos - step
                end
            end

        elseif cargo == 1 then -- the right lift
            if right_pos < 100 then
                right_pos = right_pos + step
            elseif right_pos == 100 then
                if gocrazy_beyond == GOCRAZY_BEYOND then
                    print( "Going BEYONDly crazy" )
                    gocrazy_beyond = 0
                    right_pos = right_pos + step
                else
                    gocrazy_beyond = gocrazy_beyond + 1
                    cargo = 2
                    lower_pos = 0
                    if gocrazy_slow == GOCRAZY_SLOW then
                        print( "Going SLOWly crazy" )
                        gocrazy_slow = 0
                        slow = 1
                    else
                        gocrazy_slow = gocrazy_slow + 1
                        slow = 0
                    end
                end
            elseif right_pos < 110 then
                right_pos = right_pos + step
            else
                auto = false
                blinkerror = 0
                error_overrun = true
            end

            if auto and left_pos > 0 then
                left_pos = left_pos - step
            end

        elseif cargo == 2 then -- the lower conveyor
            if lower_pos < 100 then
                lower_pos = lower_pos + step
            elseif left_pos == 0 then
                cargo = 3
            else
                -- should not happen
                auto = false
            end

            if auto then
                right_pos, slow = moveright(right_pos, slow, step)
                if left_pos > 0 then
                    left_pos = left_pos - step
                end
            end

        else  -- the left lift
            if left_pos < 100 then
                left_pos = left_pos + step
            else
                cargo = 0
                upper_pos = 0
            end

            right_pos, slow = moveright(right_pos, slow, step)

        end

    -- MANUAL MODE
    elseif get_requested_value( variables.reqauto ) then
        if right_pos + left_pos == 0 and
            (( cargo == 1 or cargo == 3 ) or
             ( cargo == 0 and upper_pos == 0 ) or
             ( cargo == 2 and lower_pos == 0 )) then
            auto = true
            print( "Switching to AUTO mode" )
        end
    else
        if is_requested_value_dirty( variables.upper_go ) then
            upper_go = get_requested_value( variables.upper_go )
        end
        if is_requested_value_dirty( variables.lower_go ) then
            lower_go = get_requested_value( variables.lower_go )
        end
        if is_requested_value_dirty( variables.right_go_down ) then
            right_go_down = get_requested_value( variables.right_go_down )
        end
        if is_requested_value_dirty( variables.right_go_up ) then
            right_go_up = get_requested_value( variables.right_go_up )
        end
        if is_requested_value_dirty( variables.left_go_down ) then
            left_go_down = get_requested_value( variables.left_go_down )
        end
        if is_requested_value_dirty( variables.left_go_up ) then
            left_go_up = get_requested_value( variables.left_go_up )
        end

        if upper_go then
            if cargo == 0 then
                if upper_pos < 100 then
                    upper_pos = upper_pos + step
                else
                    upper_go = false
                    if right_pos == 0 then
                        cargo = 1
                    else
                        upper_go = false
                    end
                end
            end
        end

        if right_go_down then
            error_tooslow = false
            if right_pos < 100 then
                right_pos = right_pos + step
            else
                right_go_down = false
                if cargo == 1 then
                    cargo = 2
                    lower_pos = 0
                end
            end
        end

        if right_go_up then
            error_tooslow = false
            if right_pos > 0 then
                right_pos = right_pos - step
                if right_pos == 100 then
                    error_overrun = false
                    right_go_up = false
                    cargo = 2
                    lower_pos = 0
                end
            else
                right_go_up = false
                if upper_pos == 100 and cargo == 0 then
                    cargo = 1
                end
            end
        end

        if lower_go then
            if cargo == 2 then
                if lower_pos < 100 then
                    lower_pos = lower_pos + step
                else
                    lower_go = false
                    if left_pos == 0 then
                        cargo = 3
                    else
                        lower_go = false
                    end
                end
            end
        end

        if left_go_up then
            if left_pos < 100 then
                left_pos = left_pos + step
            else
                left_go_up = false
                if cargo == 3 then
                    cargo = 0
                    upper_pos = 0
                end
            end
        end

        if left_go_down then
            if left_pos > 0 then
                left_pos = left_pos - step
            else
                left_go_down = false
                if lower_pos == 100 and cargo == 2 then
                    cargo = 3
                end
            end
        end
    end

    sync_requested_values()
    unlock_requested_values()

    set_real_value( variables.upper_cargo, cargo == 0 )
    set_real_value( variables.right_cargo, cargo == 1 )
    set_real_value( variables.lower_cargo, cargo == 2 )
    set_real_value( variables.left_cargo, cargo == 3 )
    set_real_value( variables.led_auto, auto )
    set_real_value( variables.led_manual, ( not auto ) )
    set_real_value( variables.upper_pos, upper_pos )
    set_real_value( variables.right_pos, right_pos )
    set_real_value( variables.lower_pos, 100 - lower_pos )
    set_real_value( variables.left_pos, 100 - left_pos )
    set_real_value( variables.upper_go, upper_go )
    set_real_value( variables.right_go_down, right_go_down )
    set_real_value( variables.right_go_up, right_go_up )
    set_real_value( variables.lower_go, lower_go )
    set_real_value( variables.left_go_down, left_go_down )
    set_real_value( variables.left_go_up, left_go_up )
    set_real_value( variables.left_limit_up, left_pos == 100 )
    set_real_value( variables.left_limit_down, left_pos == 0 )
    set_real_value( variables.right_limit_up, right_pos == 0 )
    set_real_value( variables.right_limit_down, right_pos == 100 )
    set_real_value( variables.led_safe,
                    ( ( ( right_pos + left_pos ) == 0 ) and
                      (( cargo == 1 or cargo == 3 ) or
                       (cargo == 0 and upper_pos == 0 ) or
                       (cargo == 2 and lower_pos == 0 )) ) )
    set_real_value( variables.error_tooslow,
                    error_tooslow and math.mod( blinkerror, 10 ) < 5 )
    set_real_value( variables.error_overrun,
                    error_overrun and math.mod( blinkerror, 10 ) < 5 )
    sync_real_values()
    sleep( REFRESH )
    blinkerror = blinkerror + 1

end

print( "Rollehnbahn Simulator finished\n" )


-- vim: set et ts=4 sw=4:
