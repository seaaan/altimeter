library(tidyverse)
library(stringr)
library(lubridate)

# # deal with data problems --------------------------------------------------
# x <- readLines("20170726")
# 
# # some readings are messed up 
# # ~250 get lost here
# x2 <- x[!stringr::str_detect(x, "\\v")]
# 
# # another 700 get lost here bc x2[2126] contains an EOF or something
# # so remove that row to save the data
# x2 <- x2[c(1:2125, 2127:2751)]
# x2 <- read.csv(textConnection(x2), stringsAsFactors = FALSE)
# 
# # some readings are still messed up, as revealed by NAs after coercion,
# # so remove those ones
# x2 <- x2 %>% 
#   separate(Date, into = c("Hour", "Minute", "Second"), sep = ":") %>%
#   # make all columns numeric and remove any NAs
#   mutate_all(.funs = as.numeric) %>% 
#   filter_all(all_vars(!is.na(.)))
# 
# # data munging ------------------------------------------------------------
# # create valid date column 
# x2 <- x2 %>% 
#   mutate(
#     Hour = str_pad(Hour, 2, pad = "0"), 
#     Minute = str_pad(Minute, 2, pad = "0"),
#     Second = str_pad(Second, 2, pad = "0"), 
#     Date = paste0("2017-07-26 ", Hour, ":", Minute, ":", Second),
#     Date = lubridate::ymd_hms(Date)
#   ) %>% 
#   select(-Hour:-Second)
# 
# # a few rows have super sharp negative values for Z
# # e.g. x2[66,]
# # these generally start with 4 or 42 where typical values are 425
# # so I think it's an incomplete data transmission error -> remove these
# x2 <- x2 %>% 
#   filter(Z > 50)
# # some sharp peaks remain in X, Y, and Z, but they're not obviously errors
# 
# # center and scale x y and z variables
# # very stable between  and 2017-07-26 20:03:30
# acceleration_background <- x2 %>% 
#   filter(Date > ymd_hms("2017-07-26 20:02:01"), 
#     Date < ymd_hms("2017-07-26 20:03:30")) %>% 
#   summarise_at(vars(X:Z), median) %>% 
#   transmute(Xbg = X, Ybg = Y, Zbg = Z)
# 
# x2 <- x2 %>% 
#   cbind(acceleration_background) %>% 
#   as_data_frame() %>% 
#   mutate(X = X / Xbg - 1, Y = Y / Ybg - 1, Z = Z / Zbg - 1) %>% 
#   select(-Xbg:-Zbg)
# 
# # adjust meters relative to ground
# x2 <- x2 %>% 
#   mutate(Meters = Meters - min(Meters)) %>% 
#   mutate(Feet = Meters * 3.28084)
# 
# between <- function(date, a, b) {
#   date > ymd_hms(a) & date < ymd_hms(b)
# }
# 
# # what kites were when?
# x2 <- x2 %>% mutate(Kite = case_when(
#   between(Date, "2017-07-26 20:04:00", "2017-07-26 20:08:01") ~ "Sled", 
#   between(Date, "2017-07-26 20:13:30", "2017-07-26 20:24:45") ~ "Rokkaku",
#   TRUE ~ "None")
# )
# 
# write_csv(x2, "20170726.csv")
x2 <- read_csv("20170726.csv")

# Plots -------------------------------------------------------------------
ggplot(x2, aes(x = Date, y = Meters)) + 
  geom_line(aes(color = Kite, group = 1))

ggplot(x2, aes(x = Date, y = as.numeric(Celsius))) + 
  geom_line(aes(color = Kite, group = 1))

# x may need to be recalibrated after 20:10
tidyr::gather(x2, Direction, Acceleration, X, Y, Z) %>%  
  ggplot(aes(x = Date, y = Acceleration)) + 
  geom_line(aes(color = Kite, group = 1)) + 
  facet_wrap(~ Direction, ncol = 1) + 
  geom_hline(yintercept = 0, alpha = 0.5)
