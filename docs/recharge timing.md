# Recharge Timing Calculations

## Equation

Hours of motor on = (Solar Panel Power / Motor Power Usage) * Hours of sunlight

`solar_power (watts) * hours_sun = total_recharge`

`motorA_power (watts) * hours_on + motorB_power (watts) * hours_on = power_consumption`

By ignoring separate motors, we can figure out the total time for motors running.

`motor_power * hours_on = power_consumption`

We want to use up all the power we gained in a day, so we

`let total_recharge = power_consumption`

which gives us

`solar_power * hours_sun = motor_power * hours_on`

and thus

`hours_on = (solar_power / motor_power) * hours_sun`

Because Solar Power is dependent on how much light the sun is shining, we can
slightly better estimations by adding additional terms on the right side...

    hours_on = (solar_power_strong / motor_power) * hours_sun_strong +
               (solar_power_weak / motor_power) * hours_sun_weak

You can add as many terms as you want here.

## Numbers

We have a 6V 1W solar panel, but the real value is somewhere around 0.8W and
if we account for some losses, we'll set it to 0.7W. Reminder: regardless of
wattage, the solar panel must be higher voltage than the battery it's recharging.
