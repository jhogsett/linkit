#!/usr/bin/python

import wx
import sys

def setup():
    wx.begin("d59a165785d20ad2a84f6a4a85f9a4a2", "94605")

def loop():
    print wx.get_daily_data()
    print wx.get_forecast_data()
    sys.exit()

if __name__ == '__main__':
    setup()
    while True:
        try:
            loop()
        except KeyboardInterrupt:
            sys.exit("\nExiting...\n")
        except Exception:
            raise

