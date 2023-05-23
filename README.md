
# Megatec simulator

This project uses an arduino (nano/uno/duemilanove, etc) to simulate the megatec protocol.

## Simulated items

- Input voltage (AC)
- Output voltage (AC)
- Battery voltage (DC)
- Internal temperature
- Input voltage gitch

## Megatec Protocol (in short)

| Command | Function |
|:--------------|-----------|
| D| Status Inquiry *disable|
| Q1| Status Inquiry|
| T| 10 Seconds Test|
| TL| Test until Battery Low|
| T<n>| Test for Specified Time Period|
| Q| Turn On/Off beep|
| S<n>| Shut Down Command|
| S<n>R<m>| Shut Down and Restore Command|
| C| Cancel Shut Down Command|
| CT| Cancel Test Command|
| I| UPS Information Command|
| F| UPS Rating Information|

#### Supported commands

- Q1
- Q
- T
- S
- C
- I
- F
- Invalid command handler

More details in reference section

## How to use

- With NUT:
    Use the file provided in etc/nut/ups.conf as start point for configuration
- Into a serial terminal
    Just issue the commands from the terminal, remember to adjust the end of line character to carriage return (CR).

## Disclaimer

- Tested on: Linux Mint 21.1 (Vera)

## Reference

- [Network UPS Tools](https://networkupstools.org/index.html)

- [Megatec Protocol information](https://networkupstools.org/protocols/megatec.html)

## Author

- [@santanamobile](https://www.github.com/santanamobile)

## License

[MIT](https://choosealicense.com/licenses/mit/)

# TODO

- Schematics
- Code clean up
- NUT Example
