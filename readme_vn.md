# Dự án Máy Enigma với ESP32

## Giới thiệu

Đây là một dự án làm máy Enigma hiện đại bằng cách sử dụng vi điều khiển ESP32. Máy Enigma là một thiết bị mã hóa được sử dụng trong Thế chiến II để mã hóa và giải mã các thông điệp bí mật. Với dự án này, bạn có thể mã hóa và giải mã thông điệp bằng giao diện web, hiển thị thông điệp trên ma trận LED và điều khiển thiết bị qua nhiều chức năng khác nhau.

## Tính năng

- **Mã hóa và Giải mã**: Mã hóa và giải mã thông điệp như máy Enigma.
- **Hiển thị Ma trận LED**: Hiển thị thông điệp trên ma trận LED với hiệu ứng cuộn.
- **Kết nối Wi-Fi**: Kết nối đến mạng Wi-Fi để truy cập giao diện web.
- **Giao diện Web**: Điều khiển máy Enigma và xem nhật ký qua giao diện web.
- **Âm thanh**: Phát âm thanh khi rotor quay và thông báo.
- **Ghi nhật ký**: Ghi lại các thông điệp và sự kiện với thời gian.

## Yêu cầu Phần cứng

- Vi điều khiển ESP32
- Ma trận LED (4 đơn vị)
- Loa nhỏ (buzzer)
- Đèn LED Xanh lá và Cam
- Mạng Wi-Fi

## Yêu cầu Phần mềm

- Arduino IDE hoặc PlatformIO
- Các thư viện cần thiết:
  - WiFi
  - WebServer
  - ESPmDNS
  - MD_Parola
  - MD_MAX72xx
  - SPI

## Kết nối Mạch

Kết nối các thành phần như sau:

- **Ma trận LED**:
  - CLK_PIN: GPIO 18
  - DATA_PIN: GPIO 23
  - CS_PIN: GPIO 5
- **Loa nhỏ**: GPIO 4
- **Đèn LED Xanh lá**: GPIO 26
- **Đèn LED Cam**: GPIO 27

## Hướng dẫn Cài đặt

1. **Cài đặt Thư viện**: Đảm bảo bạn đã cài đặt các thư viện cần thiết trong Arduino IDE hoặc PlatformIO.
2. **Cấu hình Wi-Fi**: Cập nhật biến `ssid` và `password` trong mã với thông tin mạng Wi-Fi của bạn.
3. **Tải Mã Lên**: Tải mã lên vi điều khiển ESP32 của bạn.
4. **Kết nối Phần cứng**: Kết nối các thành phần theo hướng dẫn.
5. **Bật Nguồn**: Bật ESP32 và chờ nó kết nối với mạng Wi-Fi.

## Hướng dẫn Sử dụng

### Giao diện Web

1. **Truy cập Giao diện**: Mở trình duyệt web và nhập địa chỉ IP hiển thị trên ma trận LED.
2. **Nhập Thông điệp**:
   - Nhập thông điệp của bạn và nhấn "Hiển thị Thông điệp" để hiển thị trên ma trận LED.
3. **Cài đặt Enigma**:
   - Chọn chế độ "Mã hóa" hoặc "Giải mã".
   - Đặt vị trí cho Rotor I, Rotor II và Rotor III.
   - Nhấn "Xử lý Thông điệp" để mã hóa hoặc giải mã.
4. **Bật/Tắt Hiệu Ứng Cuộn**:
   - Nhấn "Bật/Tắt Cuộn" để bật hoặc tắt hiệu ứng cuộn trên ma trận LED.
5. **Thử Lại**:
   - Nhấn "Thử Lại" để đặt lại thông điệp ban đầu.
6. **Xem Nhật ký**:
   - Xem các sự kiện và thông điệp trong phần nhật ký.

### Ví dụ Sử dụng

1. **Mã hóa Thông điệp**:
   - Chọn chế độ "Mã hóa".
   - Đặt vị trí rotor (ví dụ: A, B, C).
   - Nhập "HELLO" và nhấn "Hiển thị Thông điệp".
   - Nhấn "Xử lý Thông điệp" để mã hóa.
   - Thông điệp mã hóa sẽ hiển thị trên ma trận LED và được lưu vào nhật ký.

2. **Giải mã Thông điệp**:
   - Chọn chế độ "Giải mã".
   - Đặt vị trí rotor giống như khi mã hóa.
   - Nhấn "Xử lý Thông điệp" để giải mã.
   - Thông điệp gốc sẽ hiển thị trên ma trận LED và được lưu vào nhật ký.

## Giải thích Mã

### Thành phần Enigma

- **EnigmaRotor**: Đại diện cho rotor trong máy Enigma, xử lý tín hiệu tiến và lùi.
- **Reflector**: Đại diện cho bộ phản xạ trong máy Enigma.

### Chức năng Chính

- **playStepSound()**: Phát âm thanh khi rotor quay.
- **handleScrollToggle()**: Bật hoặc tắt hiệu ứng cuộn trên ma trận LED.
- **log(String message)**: Ghi lại thông điệp với thời gian.
- **stepRotors()**: Quay rotor theo cơ chế của Enigma.
- **processChar(char c)**: Mã hóa hoặc giải mã một ký tự.
- **handleProcessRequest()**: Xử lý yêu cầu mã hóa/giải mã từ web.
- **handleReset()**: Đặt lại thông điệp ban đầu.
- **handleLogs()**: Trả về nhật ký.
- **handleCurrentMessage()**: Trả về thông điệp hiện tại.
- **handleRoot()**: Hiển thị giao diện web.
- **handleUpdate()**: Cập nhật thông điệp để hiển thị.
- **setupWiFi()**: Kết nối Wi-Fi và hiển thị địa chỉ IP.
- **setup()**: Khởi tạo phần cứng và máy chủ.
- **loop()**: Vòng lặp chính để xử lý yêu cầu và hiển thị.

## Kết luận

Dự án Máy Enigma với ESP32 giúp bạn tạo ra một máy mã hóa hiện đại với giao diện web dễ sử dụng. Hãy làm theo hướng dẫn để bắt đầu mã hóa và giải mã thông điệp bằng máy Enigma của riêng bạn. Chúc bạn vui vẻ khám phá thế giới mật mã đầy thú vị!