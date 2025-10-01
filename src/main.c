#include "distributions.h"

// Прототипы функций
void print_array(double *arr, int size);
void test_value(const char *name, double actual, double expected, double tolerance);
void test_generation(double mu, double lambda, double v, int sample_size);
void test_mixture(MixtureParams *params, const char *test_name, 
                  double expected_mean, double expected_var, 
                  double expected_skew, double expected_kurt);
void test_empirical();
void test_bessel();
void test_basic_distribution();
void test_mixture_distributions();
void run_all_tests();
void show_menu();

// Глобальные переменные для настроек
int sample_size = 10000;

int main() {
    srand(time(NULL));
    show_menu();
    return 0;
}

void show_menu() {
    int choice;
    
    do {
      system("clear");
        printf("\n==============================================\n");
        printf("          ТЕСТИРОВАНИЕ РАСПРЕДЕЛЕНИЙ\n");
        printf("==============================================\n");
        printf("1. Все тесты (полный прогон)\n");
        printf("2. Основное распределение (СГР)\n");
        printf("3. Смесь распределений\n");
        printf("4. Эмпирическое распределение\n");
        printf("5. Функции Бесселя\n");
        printf("6. Тест генерации случайных величин\n");
        printf("7. Настройки (размер выборки)\n");
        printf("8. Генерация данных для графиков\n");
        printf("0. Выход\n");
        printf("==============================================\n");
        printf("Выберите опцию: ");
        
        scanf("%d", &choice);
        
        switch(choice) {
            case 1:
                run_all_tests();
                break;
            case 2:
                test_basic_distribution();
                break;
            case 3:
                test_mixture_distributions();
                break;
            case 4:
                test_empirical();
                break;
            case 5:
                test_bessel();
                break;
            case 6:
                printf("\n=== ТЕСТ ГЕНЕРАЦИИ СЛУЧАЙНЫХ ВЕЛИЧИН ===\n");
                test_generation(0.0, 1.0, 1.0, sample_size);
                test_generation(5.0, 2.0, 1.0, sample_size);
                test_generation(0.0, 1.0, 0.5, sample_size);
                break;
            case 7:
                printf("\nТекущий размер выборки: %d\n", sample_size);
                printf("Введите новый размер: ");
                scanf("%d", &sample_size);
                printf("Размер выборки изменен на: %d\n", sample_size);
                break;
            case 8: { // Генерация данных для графиков
                printf("Генерация данных для построения графиков...\n");
    
                // Тест 3.1.1: Стандартное распределение
                MixtureParams params_311 = {0, 1, 1.0, 0, 0, 0, 0};
                double* sample_311 = (double*)malloc(10000 * sizeof(double));
                for (int i = 0; i < 10000; i++) {
                    sample_311[i] = generate_main(0, 1, 1.0);
                }
                PlotData* plot_311 = generate_plot_data("3.1.1", &params_311, 0, sample_311, 10000);
                save_plot_data(plot_311);
                free_plot_data(plot_311);
                free(sample_311);

                // Тест 3.1.2: Масштабирование
                MixtureParams params_312 = {0, 2, 1.0, 0, 0, 0, 0};
                double* sample_312 = (double*)malloc(10000 * sizeof(double));
                for (int i = 0; i < 10000; i++) {
                    sample_312[i] = generate_main(0, 2, 1.0);
                }
                PlotData* plot_312 = generate_plot_data("3.1.2", &params_312, 0, sample_312, 10000);
                save_plot_data(plot_312);
                free_plot_data(plot_312);
                free(sample_312);

                // Тест 3.1.3: Сдвиг-масштаб
                MixtureParams params_313 = {5, 2, 1.0, 0, 0, 0, 0};
                double* sample_313 = (double*)malloc(10000 * sizeof(double));
                for (int i = 0; i < 10000; i++) {
                    sample_313[i] = generate_main(5, 2, 1.0);
                }
                PlotData* plot_313 = generate_plot_data("3.1.3", &params_313, 0, sample_313, 10000);
                save_plot_data(plot_313);
                free_plot_data(plot_313);
                free(sample_313);
                // Тест 3.2.1: Тривиальный случай смеси
                MixtureParams params_321 = {0, 2, 1.0, 0, 2, 1.0, 0.5};
                double* sample_321 = (double*)malloc(10000 * sizeof(double));
                for (int i = 0; i < 10000; i++) {
                    sample_321[i] = generate_mixture(&params_321);
                }
                PlotData* plot_321 = generate_plot_data("3.2.1", &params_321, 1, sample_321, 10000);
                save_plot_data(plot_321);
                free_plot_data(plot_321);
                free(sample_321);
                // Тест 3.2.2: Сдвиговые преобразования
                MixtureParams params_322 = {0, 1, 1.0, 2, 1, 1.0, 0.75};
                double* sample_322 = (double*)malloc(10000 * sizeof(double));
                for (int i = 0; i < 10000; i++) {
                    sample_322[i] = generate_mixture(&params_322);
                }
                PlotData* plot_322 = generate_plot_data("3.2.2", &params_322, 1, sample_322, 10000);
                save_plot_data(plot_322);
                free_plot_data(plot_322);
                free(sample_322);
                
                // Тест 3.2.3: Масштабные преобразования
                MixtureParams params_323 = {0, 1, 1.0, 0, 3, 1.0, 0.5};
                double* sample_323 = (double*)malloc(10000 * sizeof(double));
                for (int i = 0; i < 10000; i++) {
                    sample_323[i] = generate_mixture(&params_323);
                }
                PlotData* plot_323 = generate_plot_data("3.2.3", &params_323, 1, sample_323, 10000);
                save_plot_data(plot_323);
                free_plot_data(plot_323);
                free(sample_323);
                
                // Тест 3.2.4: Разные параметры формы
                MixtureParams params_324 = {0, 1, 0.5, 0, 1, 2.0, 0.5};
                double* sample_324 = (double*)malloc(10000 * sizeof(double));
                for (int i = 0; i < 10000; i++) {
                    sample_324[i] = generate_mixture(&params_324);
                }
                PlotData* plot_324 = generate_plot_data("3.2.4", &params_324, 1, sample_324, 10000);
                save_plot_data(plot_324);
                free_plot_data(plot_324);
                free(sample_324);
                printf("Все файлы данных сгенерированы!\n");
                break;
                }
            case 0:
                printf("Выход...\n");
                break;
            default:
                printf("Неверный выбор! Попробуйте снова.\n");
        }
        
        if (choice != 0) {
            printf("\nНажмите Enter для продолжения...");
            while (getchar() != '\n'); // Очистка буфера
            getchar(); // Ожидание Enter
        }
        
    } while (choice != 0);
}

void run_all_tests() {
    printf("\n=== ЗАПУСК ВСЕХ ТЕСТОВ ===\n");
    
    test_basic_distribution();
    test_mixture_distributions();
    test_empirical();
    test_bessel();
    
    printf("\n=== ТЕСТ ГЕНЕРАЦИИ ===\n");
    test_generation(0.0, 1.0, 1.0, sample_size);
    test_generation(5.0, 2.0, 1.0, sample_size);
    test_generation(0.0, 1.0, 0.5, sample_size);
}

void test_basic_distribution() {
    printf("\n=== ТЕСТ ОСНОВНОГО РАСПРЕДЕЛЕНИЯ (СГР) ===\n");
    
    // Тестируем плотность распределения при x=0
    double mu = 0.0, lambda = 1.0, v = 1.0;
    double density = pdf_main(0.0, mu, lambda, v);
    printf("Плотность f(0) при v=1.0: %.6f\n", density);
    test_value("f(0) должна быть ~0.306", density, 0.306, 0.01);
    
    // Тестируем моменты распределения
    double mean, variance, skewness, kurtosis;
    moments_main(mu, lambda, v, &mean, &variance, &skewness, &kurtosis);
    
    printf("\nМоменты при v=1.0:\n");
    test_value("Среднее", mean, 0.0, 0.001);
    test_value("Дисперсия", variance, 2.699, 0.01);
    test_value("Асимметрия", skewness, 0.0, 0.001);
    test_value("Эксцесс", kurtosis, 1.857, 0.01);
    
    // Тестируем с другими параметрами
    printf("\n=== Тест с масштабированием (lambda=2, v=1.0) ===\n");
    moments_main(0.0, 2.0, 1.0, &mean, &variance, &skewness, &kurtosis);
    test_value("Дисперсия при lambda=2", variance, 2.699 * 4, 0.01);
    
    printf("\n=== Тест со сдвигом (mu=5, lambda=1, v=1.0) ===\n");
    moments_main(5.0, 1.0, 1.0, &mean, &variance, &skewness, &kurtosis);
    test_value("Среднее при mu=5", mean, 5.0, 0.001);
}

void test_mixture_distributions() {
    printf("\n=== ТЕСТЫ СМЕСИ РАСПРЕДЕЛЕНИЙ ===\n");
    
    // Тест 3.2.1: Тривиальный случай
    MixtureParams test1 = {0.0, 2.0, 1.0, 0.0, 2.0, 1.0, 0.75};
    test_mixture(&test1, "Тест 3.2.1: Тривиальный случай", 0.0, 10.796, 0.0, 1.857);
    
    // Тест 3.2.2: Сдвиговые преобразования
    MixtureParams test2 = {0.0, 1.0, 1.0, 2.0, 1.0, 1.0, 0.75};
    test_mixture(&test2, "Тест 3.2.2: Сдвиговые преобразования", 0.5, 3.449, 0.0, 1.857);
    
    // Тест 3.2.3: Масштабные преобразования
    MixtureParams test3 = {0.0, 1.0, 1.0, 0.0, 3.0, 1.0, 0.5};
    test_mixture(&test3, "Тест 3.2.3: Масштабные преобразования", 0.0, 13.495, 0.0, 0.0);
    
    // Тест 3.2.4: Разные параметры формы
    MixtureParams test4 = {0.0, 1.0, 0.5, 0.0, 1.0, 2.0, 0.5};
    test_mixture(&test4, "Тест 3.2.4: Разные параметры формы", 0.0, 3.186, 0.0, 0.0);
}

void test_bessel() {
    printf("\n=== ТЕСТ ФУНКЦИЙ БЕССЕЛЯ ===\n");
    printf(" v\tK₁(v)\t\tK₂(v)\t\tK₃(v)\t\tσ²=K₂/K₁\n");
    printf("------------------------------------------------------------\n");
    
    double test_v[] = {0.1, 0.5, 1.0, 2.0, 3.0, 5.0};
    for (int i = 0; i < 6; i++) {
        double v = test_v[i];
        double k1 = bessel_k(1.0, v);
        double k2 = bessel_k(2.0, v);
        double k3 = bessel_k(3.0, v);
        double variance = (k1 > 0) ? (k2 / k1) : 0.0;
        
        printf("%.1f\t%.6f\t%.6f\t%.6f\t%.3f\n", v, k1, k2, k3, variance);
    }
}

// Реализации вспомогательных функций (остаются без изменений)
void print_array(double *arr, int size) {
    for (int i = 0; i < size; i++) {
        printf("%.2f ", arr[i]);
    }
    printf("\n");
}

void test_value(const char *name, double actual, double expected, double tolerance) {
    double diff = fabs(actual - expected);
    printf("%s: %.6f (ожидалось: %.6f) - %s\n", 
           name, actual, expected,
           (diff <= tolerance) ? "OK" : "FAIL");
}

void test_generation(double mu, double lambda, double v, int sample_size) {
    printf("\n=== Тест генерации (mu=%.1f, lambda=%.1f, v=%.1f, n=%d) ===\n", 
           mu, lambda, v, sample_size);
    
    double *sample = malloc(sample_size * sizeof(double));
    if (!sample) {
        printf("Ошибка выделения памяти!\n");
        return;
    }
    
    for (int i = 0; i < sample_size; i++) {
        sample[i] = generate_main(mu, lambda, v);
    }
    
    double mean, variance, skewness, kurtosis;
    moments_empirical(sample, sample_size, &mean, &variance, &skewness, &kurtosis);
    
    printf("Эмпирические моменты:\n");
    printf("Среднее: %.3f\n", mean);
    printf("Дисперсия: %.3f\n", variance);
    printf("Асимметрия: %.3f\n", skewness);
    printf("Эксцесс: %.3f\n", kurtosis);
    
    double theory_mean, theory_variance, theory_skewness, theory_kurtosis;
    moments_main(mu, lambda, v, &theory_mean, &theory_variance, &theory_skewness, &theory_kurtosis);
    
    printf("\nТеоретические моменты:\n");
    printf("Среднее: %.3f\n", theory_mean);
    printf("Дисперсия: %.3f\n", theory_variance);
    printf("Асимметрия: %.3f\n", theory_skewness);
    printf("Эксцесс: %.3f\n", theory_kurtosis);
    
    printf("\nПроверка близости:\n");
    test_value("Среднее", mean, theory_mean, fabs(theory_mean * 0.1) + 0.1);
    test_value("Дисперсия", variance, theory_variance, theory_variance * 0.15);
    test_value("Асимметрия", skewness, theory_skewness, 0.2);
    test_value("Эксцесс", kurtosis, theory_kurtosis, 0.3);
    
    free(sample);
}

void test_mixture(MixtureParams *params, const char *test_name, 
                  double expected_mean, double expected_var, 
                  double expected_skew, double expected_kurt) {
    printf("\n=== %s ===\n", test_name);
    
    double mean, variance, skewness, kurtosis;
    moments_mixture(params, &mean, &variance, &skewness, &kurtosis);
    
    printf("Теоретические моменты смеси:\n");
    printf("Среднее: %.3f (ожидалось: %.3f)\n", mean, expected_mean);
    printf("Дисперсия: %.3f (ожидалось: %.3f)\n", variance, expected_var);
    printf("Асимметрия: %.3f (ожидалось: %.3f)\n", skewness, expected_skew);
    printf("Эксцесс: %.3f (ожидалось: %.3f)\n", kurtosis, expected_kurt);
    
    const int s_size = 50000;
    double *sample = malloc(s_size * sizeof(double));
    for (int i = 0; i < s_size; i++) {
        sample[i] = generate_mixture(params);
    }
    
    double emp_mean, emp_var, emp_skew, emp_kurt;
    moments_empirical(sample, s_size, &emp_mean, &emp_var, &emp_skew, &emp_kurt);
    
    printf("\nЭмпирические моменты (n=%d):\n", s_size);
    printf("Среднее: %.3f\n", emp_mean);
    printf("Дисперсия: %.3f\n", emp_var);
    printf("Асимметрия: %.3f\n", emp_skew);
    printf("Эксцесс: %.3f\n", emp_kurt);
    
    free(sample);
}

void test_empirical() {
    printf("\n=== ТЕСТ ЭМПИРИЧЕСКОГО РАСПРЕДЕЛЕНИЯ ===\n");
    
    // Часть 1: Базовый тест эмпирических функций
    printf("\n--- Часть 1: Базовые эмпирические функции ---\n");
    double test_sample[] = {1.0, 2.0, 3.0, 4.0, 5.0};
    int size = 5;

    printf("Тестовая выборка: ");
    print_array(test_sample, size);

    double mean, variance, skewness, kurtosis;
    moments_empirical(test_sample, size, &mean, &variance, &skewness, &kurtosis);

    printf("Эмпирические моменты:\n");
    printf("Среднее: %.2f\n", mean);
    printf("Дисперсия: %.2f\n", variance);
    printf("Асимметрия: %.2f\n", skewness);
    printf("Эксцесс: %.2f\n", kurtosis);

    printf("\nСгенерированные значения из эмпирического распределения:\n");
    for (int i = 0; i < 10; i++) {
        double value = generate_empirical(test_sample, size);
        printf("%.2f ", value);
    }
    printf("\n");

    // Часть 2: Сравнение с теоретическим распределением
    printf("\n--- Часть 2: Сравнение с СГР (v=1.0) ---\n");
    
    // Генерируем выборку из основного распределения
    const int sample_size = 10000;
    double *sample = malloc(sample_size * sizeof(double));
    if (!sample) {
        printf("Ошибка выделения памяти!\n");
        return;
    }
    
    for (int i = 0; i < sample_size; i++) {
        sample[i] = generate_main(0.0, 1.0, 1.0);
    }
    
    // Вычисляем теоретическую и эмпирическую плотность в ключевых точках
    double test_points[] = {-2.0, -1.5, -1.0, -0.5, 0.0, 0.5, 1.0, 1.5, 2.0};
    int n_points = sizeof(test_points) / sizeof(test_points[0]);
    
    printf("Сравнение плотностей в точках (n=%d):\n", sample_size);
    printf(" x\tТеор. f(x)\tЭмп. f(x)\tОтн. ошибка\n");
    printf("------------------------------------------------\n");
    
    for (int i = 0; i < n_points; i++) {
        double x = test_points[i];
        double theory_pdf = pdf_main(x, 0.0, 1.0, 1.0);
        double empirical_pdf = pdf_empirical(x, sample, sample_size);
        double error = fabs(theory_pdf - empirical_pdf) / theory_pdf * 100;
        
        printf("%.1f\t%.6f\t%.6f\t%.1f%%\n", x, theory_pdf, empirical_pdf, error);
    }
    
    // Часть 3: Тест 3.3.2 - Генерация из эмпирического распределения
    printf("\n--- Часть 3: Тест 3.3.2 - Генерация из эмпирического распределения ---\n");
    
    double *new_sample = malloc(sample_size * sizeof(double));
    if (!new_sample) {
        printf("Ошибка выделения памяти!\n");
        free(sample);
        return;
    }
    
    for (int i = 0; i < sample_size; i++) {
        new_sample[i] = generate_empirical(sample, sample_size);
    }
    
    // Сравниваем моменты исходной и новой выборки
    double orig_mean, orig_var, orig_skew, orig_kurt;
    double new_mean, new_var, new_skew, new_kurt;
    
    moments_empirical(sample, sample_size, &orig_mean, &orig_var, &orig_skew, &orig_kurt);
    moments_empirical(new_sample, sample_size, &new_mean, &new_var, &new_skew, &new_kurt);
    
    printf("Исходная выборка:  M=%.3f, D=%.3f, γ1=%.3f, γ2=%.3f\n", 
           orig_mean, orig_var, orig_skew, orig_kurt);
    printf("Новая выборка:     M=%.3f, D=%.3f, γ1=%.3f, γ2=%.3f\n", 
           new_mean, new_var, new_skew, new_kurt);
    
    // Проверяем близость характеристик
    printf("\nПроверка близости характеристик:\n");
    test_value("Среднее", new_mean, orig_mean, fabs(orig_mean * 0.1) + 0.1);
    test_value("Дисперсия", new_var, orig_var, orig_var * 0.15);
    test_value("Асимметрия", new_skew, orig_skew, 0.2);
    test_value("Эксцесс", new_kurt, orig_kurt, 0.3);
    
    // Часть 4: Визуальная проверка распределения
    printf("\n--- Часть 4: Визуальная проверка распределения ---\n");
    printf("Для построения графиков используй данные:\n");
    printf("x,Теоретическая,Эмпирическая\n");
    for (double x = -3.0; x <= 3.0; x += 0.5) {
        double theory = pdf_main(x, 0.0, 1.0, 1.0);
        double empirical = pdf_empirical(x, sample, sample_size);
        printf("%.1f,%.6f,%.6f\n", x, theory, empirical);
    }
    
    // Освобождаем память
    free(sample);
    free(new_sample);
    
    printf("\n=== ТЕСТ ЭМПИРИЧЕСКОГО РАСПРЕДЕЛЕНИЯ ЗАВЕРШЕН ===\n");
}