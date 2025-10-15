import numpy as np
import matplotlib.pyplot as plt
import os
from scipy.stats import gaussian_kde
import scipy.stats as stats

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

def create_mega_plot_332():
    """Мега сложный график для теста 3.3.2 с тремя распределениями"""
    print("Создание мега сложного графика 3.3.2...")
    
    if not os.path.exists("special_plots"):
        os.makedirs("special_plots")
    
    # Читаем все три распределения
    try:
        main_data = read_plot_data("plot_data_3.3.2_main.txt")
        empirical_main_data = read_plot_data("plot_data_3.3.2_empirical_main.txt")
        empirical_bootstrap_data = read_plot_data("plot_data_3.3.2_empirical_bootstrap.txt")
    except FileNotFoundError as e:
        print(f"Ошибка: {e}")
        print("Сначала запусти C программу для генерации данных!")
        return
    
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(16, 6))
    
    # ЛЕВЫЙ ГРАФИК: Все три распределения вместе
    # 1. Теоретическое основное распределение
    ax1.plot(main_data['theoretical_x'], main_data['theoretical_y'], 
             'b-', linewidth=3, label='Теоретическое основное f(x)', alpha=0.8)
    
    # 2. KDE из выборки основного распределения
    if len(empirical_main_data['empirical_data']) > 0:
        kde_main = gaussian_kde(empirical_main_data['empirical_data'])
        kde_x = np.linspace(-4, 4, 200)
        kde_y_main = kde_main(kde_x)
        ax1.plot(kde_x, kde_y_main, 'r--', linewidth=2, 
                label='KDE из выборки основного f(x)', alpha=0.8)
    
    # 3. KDE из бутстрэп-выборки
    if len(empirical_bootstrap_data['empirical_data']) > 0:
        kde_bootstrap = gaussian_kde(empirical_bootstrap_data['empirical_data'])
        kde_y_bootstrap = kde_bootstrap(kde_x)
        ax1.plot(kde_x, kde_y_bootstrap, 'g:', linewidth=2, 
                label='KDE из выборки эмпирического fe(x)', alpha=0.8)
    
    ax1.set_title('Сравнение трех распределений\nТест 3.3.2', fontsize=14, fontweight='bold')
    ax1.set_xlabel('x')
    ax1.set_ylabel('Плотность вероятности')
    ax1.legend()
    ax1.grid(True, alpha=0.3)
    ax1.set_xlim(-4, 4)
    
    # ПРАВЫЙ ГРАФИК: Гистограммы для наглядности
    # Гистограмма из основного распределения
    if len(empirical_main_data['empirical_data']) > 0:
        ax2.hist(empirical_main_data['empirical_data'], bins=30, density=True, 
                alpha=0.5, color='red', label='Выборка из основного f(x)')
    
    # Гистограмма из бутстрэп-выборки
    if len(empirical_bootstrap_data['empirical_data']) > 0:
        ax2.hist(empirical_bootstrap_data['empirical_data'], bins=30, density=True, 
                alpha=0.5, color='green', label='Выборка из эмпирического fe(x)')
    
    # Теоретическая кривая для reference
    ax2.plot(main_data['theoretical_x'], main_data['theoretical_y'], 
             'b-', linewidth=2, label='Теоретическое основное f(x)', alpha=0.8)
    
    ax2.set_title('Гистограммы выборок\nТест 3.3.2', fontsize=14, fontweight='bold')
    ax2.set_xlabel('x')
    ax2.set_ylabel('Плотность вероятности')
    ax2.legend()
    ax2.grid(True, alpha=0.3)
    ax2.set_xlim(-4, 4)
    
    # Вычисляем статистики для сравнения
    if len(empirical_main_data['empirical_data']) > 0 and len(empirical_bootstrap_data['empirical_data']) > 0:
        main_mean = np.mean(empirical_main_data['empirical_data'])
        bootstrap_mean = np.mean(empirical_bootstrap_data['empirical_data'])
        main_std = np.std(empirical_main_data['empirical_data'])
        bootstrap_std = np.std(empirical_bootstrap_data['empirical_data'])
        
        stats_text = (f'Сравнение статистик:\n'
                     f'Выборка из f(x): μ={main_mean:.3f}, σ={main_std:.3f}\n'
                     f'Выборка из fe(x): μ={bootstrap_mean:.3f}, σ={bootstrap_std:.3f}\n'
                     f'Разница средних: {abs(main_mean - bootstrap_mean):.4f}')
        
        fig.text(0.5, 0.01, stats_text, ha='center', fontsize=11, 
                bbox=dict(boxstyle="round,pad=0.5", facecolor="lightgray", alpha=0.8))
    
    plt.tight_layout()
    plt.subplots_adjust(bottom=0.12)
    plt.savefig("special_plots/MEGA_PLOT_3.3.2_COMPARISON.png", dpi=300, bbox_inches='tight')
    plt.close()
    
    # Дополнительный график: ошибки
    create_error_plot(main_data, empirical_main_data, empirical_bootstrap_data)
    
    print("Мега сложный график 3.3.2 создан!")

def create_error_plot(main_data, empirical_main_data, empirical_bootstrap_data):
    """График ошибок между распределениями"""
    fig, ax = plt.subplots(figsize=(12, 6))
    
    # Вычисляем KDE для обоих эмпирических распределений
    kde_main = gaussian_kde(empirical_main_data['empirical_data'])
    kde_bootstrap = gaussian_kde(empirical_bootstrap_data['empirical_data'])
    
    # Общий диапазон x
    x_common = np.linspace(-4, 4, 200)
    
    # Теоретическая плотность в этих точках (интерполяция)
    theoretical_interp = np.interp(x_common, main_data['theoretical_x'], main_data['theoretical_y'])
    
    # KDE плотности
    kde_y_main = kde_main(x_common)
    kde_y_bootstrap = kde_bootstrap(x_common)
    
    # Вычисляем ошибки
    error_main = np.abs(theoretical_interp - kde_y_main)
    error_bootstrap = np.abs(theoretical_interp - kde_y_bootstrap)
    
    # График ошибок
    ax.plot(x_common, error_main, 'r-', linewidth=2, 
            label='Ошибка |f(x) - KDE из f(x)|', alpha=0.8)
    ax.plot(x_common, error_bootstrap, 'g-', linewidth=2, 
            label='Ошибка |f(x) - KDE из fe(x)|', alpha=0.8)
    
    ax.set_title('Ошибки аппроксимации теоретического распределения\nТест 3.3.2', 
                 fontsize=14, fontweight='bold')
    ax.set_xlabel('x')
    ax.set_ylabel('Абсолютная ошибка')
    ax.legend()
    ax.grid(True, alpha=0.3)
    ax.set_ylim(bottom=0)
    
    # Средние ошибки
    mean_error_main = np.mean(error_main)
    mean_error_bootstrap = np.mean(error_bootstrap)
    
    stats_text = f'Средняя ошибка:\nKDE из f(x): {mean_error_main:.4f}\nKDE из fe(x): {mean_error_bootstrap:.4f}'
    ax.text(0.02, 0.98, stats_text, transform=ax.transAxes, verticalalignment='top',
            bbox=dict(boxstyle="round,pad=0.3", facecolor="white", alpha=0.8))
    
    plt.tight_layout()
    plt.savefig("special_plots/ERROR_PLOT_3.3.2.png", dpi=300, bbox_inches='tight')
    plt.close()
    print("График ошибок создан!")

def create_special_plots():
    """Создает специальные графики"""
    print("Создание специальных графиков...")
    
    # Создаем мега график 3.3.2
    create_mega_plot_332()
    
    # Графики для 3.3.1.1 - 3.3.1.4 (если нужны)
    test_cases = ["3.3.1.1", "3.3.1.2", "3.3.1.3", "3.3.1.4"]
    
    for test_case in test_cases:
        filename = f"plot_data_{test_case}.txt"
        if os.path.exists(filename):
            print(f"Обработка {filename}...")
            # Можно добавить создание отдельных графиков если нужно

if __name__ == "__main__":
    create_special_plots()
    print("\n✅ Все графики созданы в папке 'special_plots'!")