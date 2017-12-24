
weather_conditions = { 
0: "",		    # segment spacer
1: "blk",           # segment filler
2: "red:dim:bre",   # day spacer

# storm
200: "pur:dim",     # thunderstorm with light rain
210: "pur:dim",     # light thunderstorm
230: "pur:dim",     # thunderstorm with light drizzle
201: "pur",         # thunderstorm with rain
211: "pur",         # thunderstorm
221: "pur:bre",     # ragged thunderstorm
231: "pur:bre",     # thunderstorm with drizzle
202: "pur:bli",     # thunderstorm with heavy rain
212: "pur:bli",     # heavy thunderstorm
232: "pur:bli",     # thunderstorm with heavy drizzle

# drizzle
300: "cyn:dim",     # light intensity drizzle
310: "cyn:dim",     # light intensity drizzle rain
301: "cyn",         # drizzle
311: "cyn",         # drizzle rain
321: "cyn:bre",     # shower drizzle
302: "cyn:bli",     # heavy intensity drizzle
312: "cyn:bli",     # heavy intensity drizzle rain
313: "cyn:bli",     # shower rain and drizzle
314: "cyn:bli",     # heavy shower rain and drizzle

# rain
500: "grn:dim",     # light rain
520: "grn:dim",     # light intensity shower rain
501: "grn",         # moderate rain
511: "grn",         # freezing rain
521: "grn:bre",     # shower rain
531: "grn:bew",     # ragged shower rain
502: "grn:bli",     # heavy intensity rain
522: "grn:bli",     # heavy intensity shower rain
503: "grn:bla",     # very heavy rain
504: "3,0:dyn:bld", # extreme rain

# snow
600: "wht",         # light snow
611: "wht",         # sleet
615: "wht",         # light rain and snow
620: "wht",         # light shower snow
601: "wht:bre",     # snow
612: "wht:bre",     # shower sleet
616: "wht:bre",     # rain and snow
621: "wht:bre",     # shower snow
602: "wht:bli",     # heavy snow
622: "wht:bli",     # heavy shower snow

# haze
701: "org:dim",     # mist
711: "org",         # smoke
721: "org",         # haze
731: "org:bre",     # sand, dust whirls
741: "org:bre",     # fog
751: "org:bla",     # sand
761: "org:bla",     # dust  
762: "org:bla",     # volcanic ash
771: "org:bla",     # squalls
781: "0,1:dyn:bld", # tornado

# clear
800: "yel:dim",     # clear

# clouds
801: "lbl:dim",     # few clouds
802: "gry",         # scattered clouds
803: "dgr",         # broken clouds
804: "dgr:dim",     # overcast clouds

# wind
951: "blu:dim",     # calm
952: "blu:dim",     # light breeze
953: "blu:dim",     # gentle breeze
954: "blu",         # moderate breeze
955: "blu",         # fresh breeze
956: "blu:bre",     # strong breeze
957: "blu:bre",     # high wind, near gale
958: "blu:bli",     # gale
959: "blu:bla",     # severe gale
960: "blu:bla",     # storm
961: "0,4:dyn:bld", # violent storm
962: "0,4:dyn:bld", # hurricane

# extreme
900: "7,2:dyn:bld", # tornado
901: "7,4:dyn:bld", # tropical storm
902: "7,1:dyn:bld", # hurricane
903: "7,6:dyn:bld", # cold
904: "7,5:dyn:bld", # hot
905: "7,8:dyn:bld", # windy
906: "7,0:dyn:bld"  # hail
}

