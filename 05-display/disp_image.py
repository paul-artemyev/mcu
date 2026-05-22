import time
import serial
from PIL import Image

# Настройки COM-порта (измените COM-порт на ваш)
SERIAL_PORT = 'COM6'  # Укажите ваш COM-порт
BAUD_RATE = 115200

# Путь к изображению (файл лежит в той же папке что и скрипт)
img_path = 'photo.png'  # ← ИСПРАВЛЕНО!

def main():
    # Открываем изображение
    print(f"Загрузка изображения: {img_path}")
    try:
        image = Image.open(img_path)
    except FileNotFoundError:
        print(f"ОШИБКА: Файл '{img_path}' не найден!")
        print("Убедитесь, что файл photo.png лежит в папке со скриптом")
        return
    
    # Конвертируем в RGB и изменяем размер до 320x240
    image = image.convert('RGB')
    image = image.resize((320, 240))  # Принудительно изменяем размер
    print(f"Размер изображения: {image.size[0]} x {image.size[1]}")
    
    # Открываем COM-порт
    ser = None
    try:
        ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
        time.sleep(2)  # Ждём инициализации
        print(f"Подключено к {SERIAL_PORT}")
        
        # Очищаем экран перед загрузкой
        print("Очистка экрана...")
        ser.write(b'disp_screen 0x000000\n')
        time.sleep(0.5)
        
        # Отправляем каждый пиксель
        total_pixels = 320 * 240
        count = 0
        start_time = time.time()
        
        print("Отправка изображения...")
        
        for y in range(240):
            for x in range(320):
                # Получаем цвет пикселя
                r, g, b = image.getpixel((x, y))
                
                # Формируем цвет в формате RGB888 (0xRRGGBB)
                color = (r << 16) | (g << 8) | b
                
                # Отправляем команду
                command = f"disp_px {x} {y} {color:06X}\n"
                ser.write(command.encode('utf-8'))
                
                count += 1
                
                # Прогресс (каждые 10000 пикселей)
                if count % 10000 == 0:
                    percent = (count / total_pixels) * 100
                    print(f"  Прогресс: {count}/{total_pixels} ({percent:.1f}%)")
            
            # Показываем прогресс по строкам
            if y % 20 == 0:
                percent = (count / total_pixels) * 100
                print(f"  Строка {y}/240 ({percent:.1f}%)")
        
        elapsed = time.time() - start_time
        print(f"\nИзображение загружено!")
        print(f"Отправлено пикселей: {count}")
        print(f"Время загрузки: {elapsed:.2f} секунд")
        
    except serial.SerialException as e:
        print(f"Ошибка COM-порта: {e}")
        print("Проверьте, что Pico подключен и COM-порт правильный")
    except Exception as e:
        print(f"Ошибка: {e}")
        
    finally:
        if ser and ser.is_open:
            time.sleep(0.1)  # Пауза перед закрытием
            ser.close()
            print("COM-порт закрыт")

if __name__ == "__main__":
    main()