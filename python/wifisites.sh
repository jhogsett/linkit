#!/bin/sh

iwpriv ra0 set SiteSurvey=1
sleep 5
iwpriv ra0 get_site_survey ra0

