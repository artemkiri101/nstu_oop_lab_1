import numpy as np
import matplotlib.pyplot as plt
import glob
import os

def read_plot_data(filename):
    """Читает данные из файла, сгенерированного C программой"""
    theoretical_x = []
    theoretical_y = []
    empirical_data = []
    
    with open(filename, 'r') as f:
        lines = f.readlines()
    
    # Парсим метаданные
    title = lines[0].strip('# \n')
    points_count = int(lines[1].split(':')[1].strip())
    empirical_size = int(lines[2].split(':')[1].strip())
    
    # Читаем теоретические данные
    theoretical_section = False
    empirical_section = False
    
    for line in lines[3:]:
        line = line.strip()
        if not line:
            continue
            
        if line.startswith('# empirical_data:'):
            empirical_section = True
            theoretical_section = False
            continue
            
        if empirical_section:
            if line and not line.startswith('#'):
                empirical_data.append(float(line))
        else:
            if line and not line.startswith('#'):
                parts = line.split()
                if len(parts) >= 2:
                    theoretical_x.append(float(parts[0]))
                    theoretical_y.append(float(parts[1]))
                    theoretical_section = True
    
    return {
        'title': title,
        'theoretical_x': np.array(theoretical_x),
        'theoretical_y': np.array(theoretical_y),
        'empirical_data': np.array(empirical_data),
        'points_count': points_count,
        'empirical_size': empirical_size
    }

def create_plot(data, output_dir="plots"):
    """Создает график на основе данных"""
    # Создаем директорию для графиков, если её нет
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)
    
    plt.figure(figsize=(12, 8))
    
    # Теоретическая кривая
    plt.plot(data['theoretical_x'], data['theoretical_y'], 
             'b-', linewidth=3, label='Теоретическая плотность', alpha=0.8)
    
    # Эмпирическая гистограмма (если есть данные)
    if len(data['empirical_data']) > 0:
        plt.hist(data['empirical_data'], bins=50, density=True, 
                alpha=0.6, color='orange', label='Эмпирическая гистограмма')
    
    # Настройки графика
    plt.title(f"Тест {data['title']}", fontsize=14, fontweight='bold')
    plt.xlabel('x', fontsize=12)
    plt.ylabel('Плотность f(x)', fontsize=12)
    plt.legend(fontsize=11)
    plt.grid(True, alpha=0.3)
    
    # Автоматическая настройка пределов
    x_data = data['theoretical_x']
    if len(data['empirical_data']) > 0:
        x_data = np.concatenate([x_data, data['empirical_data']])
    
    x_range = x_data.max() - x_data.min()
    plt.xlim(x_data.min() - 0.1 * x_range, x_data.max() + 0.1 * x_range)
    
    # Сохраняем график
    safe_title = data['title'].replace('.', '_')
    output_filename = os.path.join(output_dir, f"plot_{safe_title}.png")
    plt.tight_layout()
    plt.savefig(output_filename, dpi=300, bbox_inches='tight')
    plt.close()
    
    print(f"График сохранен: {output_filename}")

def main():
    """Основная функция для генерации всех графиков"""
    print("Поиск файлов с данными...")
    
    # Ищем все файлы с данными
    data_files = glob.glob("plot_data_*.txt")
    
    if not data_files:
        print("Файлы с данными не найдены!")
        print("Сначала запустите C программу и выберите опцию генерации графиков.")
        return
    
    print(f"Найдено {len(data_files)} файлов с данными")
    
    # Создаем графики для каждого файла
    for data_file in sorted(data_files):
        print(f"Обработка {data_file}...")
        try:
            data = read_plot_data(data_file)
            create_plot(data)
        except Exception as e:
            print(f"Ошибка при обработке {data_file}: {e}")
    
    print("\nВсе графики успешно созданы в папке 'plots'!")

if __name__ == "__main__":
    main()