# Internal Reference

INTERNAL reference is 1.0~1.2v. Arduino docs say it's 1.1v, but it's not quite right.

The internal reference is based on the bandgap inside the chip.

It's stable for each chip, but across each chip, the number could be slightly different. In other words, two different chips will have different numbers.

You can test it by doing an analogRead with the INTERNAL ref against a known stable voltage, and then figuring out what the reading is (writing to Serial). Could also write to EEPROM, but remember to divide by 4.

internalRefV = maxAnalogRead * voltageGiven / analogReadValue

example:

voltage = 0.8v = 3.2v/4 (divide by 4 via divider)
analogRead = 200

internalRefV = 255 * 0.8 / 200 = 1v

resolution = internalRefV / maxAnalogRead

resolution = 1v/1023 = 0.000977517v per increment

# Levels
NAME	voltage	v/4	analogRead

OVERCHARGE	3.6	0.9	837
SOLAR	3.5	0.875	814
FULL	3.4	0.85	791
CHARGED	3.3	0.825	767
NOMINAL	3.2	0.8	744
DRAINED	3.1	0.775	721
CUTOFF	3.0	0.75	697

TOLERANCE 0.05	50

# Math

1.1v / 1023 = 0.001075v

Battery voltage is 2.7-3.6v, but should only be 3.0-3.5 when in use. Divided by ~4 via a pot.

Solar panel is 6v 1W (~0.166mA) (actual, 6.3~6.7 ~130mA), bucked with a mini360 to 3.5v (hopefully giving >250mA).

# Cycles

NAME	current(mA)	analogWrite	baseCurrent

MAXIMUM	200	1023	*
HIGH	100	*	*
MEDIUM	90	*	*
LOW	85	*	*
MIN	80	*	*
OFF	0	0	0

Should be careful about running on LOW and MIN, because if there's a lot of load (high water levels, low air, etc) then it's going to stall the motor.

Because the voltage is varying, we can't quite guarantee the exact current output...

We start the motor off on MAX, and then drop the voltage down to its desired target.

Assuming a gain of ~50 over the range and a 1.1k base resistor:

4.3*(analogOut)/1100 = base current

50*4.3*(analogOut)/1100 = collector-emitter current

1.1k
	aout	current(mA)
HIGH	500	100	97
MEDIUM	450	90	88
LOW	435	85	85
MIN	400	80	78

* these numbers need to be verified for real

	aout	Emit	1.3k	1.2k	1.1k
MAX	1023	200+	169	183	200
HIGH	600	100	99	107	117
MEDIUM	550	90	91	98	107
LOW	475	85	79	85	93
LOW	510	85	84	91	100
MIN	480	80	79	86	94
MIN	450	80	74	81	88
