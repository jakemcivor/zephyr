.. _lps22hb:

LPS22HB MEMS Nano Pressure Sensor Absolute Digital Output Barometer
##########################

Overview
********

A sensor application that demonstrates how to peroidically read data from
the LPS22HB MEMS Nano Pressure Sensor and display it to the console.

Requirements
************

This sample uses the LPS22HB sensor controlled using the I2C interface.

References
**********

 - LPS22HB: http://www.st.com/en/mems-and-sensors/lps22hb.html

Building and Running
********************

This project configures the lps22hb sensor on the :ref:`disco_l475_iot1` 
board to measure the absolute pressure. The lps22hb sensor also records
temperature.

.. zephyr-app-commands::
   :zephyr-app: samples/sensors/lps22hb
   :board: disco_l475_iot1
   :goals: build
   :compact:

Sample Output
=============

.. code-block:: console

    Pressure:100.455 kPa
    Temperature:29.0 C
    Pressure:100.454 kPa
    Temperature:29.0 C
    Pressure:100.451 kPa
    Temperature:29.0 C
    Pressure:100.455 kPa
    Temperature:29.0 C


<repeats endlessly>
