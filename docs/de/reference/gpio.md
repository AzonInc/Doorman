# GPIO Pinbelegung

Der Doorman-S3 nutzt bestimmte GPIO-Pins für spezielle Funktionen und bietet zusätzliche GPIO-Pins für deine eigenen Sensoren und Geräte.

| GPIO  | Verwensungszweck |
| :---  | :---  |
| GPIO01 | Rote Status-LED |
| GPIO02 | WS2812B RGB Status-LED |
| GPIO08 | TC Bus TX - Kurzschluss des Busses zu Ground |
| GPIO09 | TC Bus RX - Liest Bus-Daten / ADC-Eingang (mit integriertem Spannungsteiler - 1M+160K) |
| GPIO10 | Rev >= 1.5 - Verbunden mit GPIO09 -  Alternativer ADC Eingang |
| GPIO40 | Freier I/O |
| GPIO41 | 10K Onboard-Widerstand für externen Button |
| GPIO42 | Relais für analogen Türöffner oder Licht |
| GPIO48 | Freier I/O |