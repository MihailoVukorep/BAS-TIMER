#!/bin/bash

# Return a colour that contrasts with the given colour
# Bash only does integer division, so keep it integral
function contrast_colour {
    local r g b luminance
    colour="$1"

    if (( colour < 16 )); then # Initial 16 ANSI colours
        (( colour == 0 )) && printf "15" || printf "0"
        return
    fi

    # Greyscale # rgb_R = rgb_G = rgb_B = (number - 232) * 10 + 8
    if (( colour > 231 )); then # Greyscale ramp
        (( colour < 244 )) && printf "15" || printf "0"
        return
    fi

    # All other colours:
    # 6x6x6 colour cube = 16 + 36*R + 6*G + B  # Where RGB are [0..5]
    # See http://stackoverflow.com/a/27165165/5353461

    # r=$(( (colour-16) / 36 ))
    g=$(( ((colour-16) % 36) / 6 ))
    # b=$(( (colour-16) % 6 ))

    # If luminance is bright, print number in black, white otherwise.
    # Green contributes 587/1000 to human perceived luminance - ITU R-REC-BT.601
    (( g > 2)) && printf "0" || printf "15"
    return

    # Uncomment the below for more precise luminance calculations

    # # Calculate percieved brightness
    # # See https://www.w3.org/TR/AERT#color-contrast
    # # and http://www.itu.int/rec/R-REC-BT.601
    # # Luminance is in range 0..5000 as each value is 0..5
    # luminance=$(( (r * 299) + (g * 587) + (b * 114) ))
    # (( $luminance > 2500 )) && printf "0" || printf "15"
}

function print_colour {
    local colour="$1" contrast
    contrast=$(contrast_colour "$1")
    printf "\e[48;5;%sm" "$colour"                # Start block of colour
    printf "\e[38;5;%sm%3d" "$contrast" "$colour" # In contrast, print number
    printf "\e[0m "                               # Reset colour
    echo ""
}

function print_block {
    print_colour "$1"
}


print_colour 196
print_colour 202
print_colour 208
print_colour 214
print_colour 220
print_colour 226
print_colour 21
print_colour 27
print_colour 33
print_colour 39
print_colour 45
print_colour 51
