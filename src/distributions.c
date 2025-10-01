#include "distributions.h"

#include <gsl/gsl_sf_bessel.h>

#define M_PI 3.14159265358979323846

// --- ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ ---

double bessel_k(double nu, double x) {
    return gsl_sf_bessel_Knu(nu, x);
}

// --- ОСНОВНОЕ РАСПРЕДЕЛЕНИЕ (СГР) ---

double pdf_main(double x, double mu, double lambda, double v) {
    if (lambda <= 0 || v <= 0) {
        return 0.0;
    }
    
    double z = (x - mu) / lambda;
    
    // Теперь используем НАСТОЯЩУЮ функцию Бесселя!
    double k1 = bessel_k(1.0, v);
    if (k1 <= 0 || isnan(k1) || isinf(k1)) {
        return 0.0;
    }
    
    double normalization = 1.0 / (2.0 * sqrt(v) * k1);
    double exponent = exp(-sqrt(1.0 + z * z));
    
    return normalization * exponent;
}

void moments_main(double mu, double lambda, double v, double *mean, double *variance, double *skewness, double *kurtosis) {
    // Проверка корректности параметров
    if (lambda <= 0 || v <= 0) {
        if (mean) *mean = 0;
        if (variance) *variance = 0;
        if (skewness) *skewness = 0;
        if (kurtosis) *kurtosis = 0;
        return;
    }
    
    // Математическое ожидание: M = mu
    if (mean) *mean = mu;
    
    // Для симметричного гиперболического распределения:
    // Асимметрия всегда равна 0 (распределение симметричное)
    if (skewness) *skewness = 0.0;
    
    // Вычисляем дисперсию и эксцесс по формулам из варианта
    double k1 = bessel_k(1.0, v);
    double k2 = bessel_k(2.0, v);
    double k3 = bessel_k(3.0, v);
    
    // Дисперсия: D = lambda^2 * (K_2(v) / K_1(v))
    if (variance) {
        *variance = lambda * lambda * (k2 / k1);
    }
    
    // Эксцесс: γ₂ = 3 * K_3(v) * K_1(v) / (K_2(v))^2 - 3
    if (kurtosis) {
        if (k2 > 0) {
            *kurtosis = 3.0 * k3 * k1 / (k2 * k2) - 3.0;
        } else {
            *kurtosis = 0.0;
        }
    }
}

// Вспомогательная функция для генерации равномерного распределения
double uniform_random() {
    return (double)rand() / RAND_MAX;
}

// Вспомогательная функция для генерации стандартной нормальной величины (метод Бокса-Мюллера)
double normal_random() {
    double u1 = uniform_random();
    double u2 = uniform_random();
    while (u1 <= 1e-10) u1 = uniform_random(); // Избегаем log(0)
    
    // Бокс-Мюллер преобразование
    return sqrt(-2.0 * log(u1)) * cos(2.0 * M_PI * u2);
}

double generate_main(double mu, double lambda, double v) {
    if (lambda <= 0 || v <= 0) {
        return 0.0;
    }
    
    double delta = (2.0 / v) * (sqrt(1.0 + v * v) - 1.0);
    double t;
    int attempts = 0;
    const int max_attempts = 1000;
    
    while (attempts < max_attempts) {
        attempts++;
        
        double r1 = uniform_random();
        double r2 = uniform_random();
        while (r1 <= 1e-12) r1 = uniform_random();
        while (r2 <= 1e-12) r2 = uniform_random();
        
        t = -2.0 / delta * log(r1);
        
        if (t <= 1e-12) continue;
        
        double left_side = -log(r2);
        double term1 = (v - delta) * t / 2.0;
        double term2 = v / (2.0 * t);
        double term3 = sqrt(v * (v - delta));
        double right_side = term1 + term2 - term3;
        
        // ИСПРАВЛЕНИЕ: если условие ВЫПОЛНЕНО, то ОТКЛОНЯЕМ и продолжаем цикл
        if (left_side <= right_side) {
            continue; // Отклоняем и начинаем заново
        }
        
        break; // Принимаем и выходим из цикла
    }
    
    if (attempts >= max_attempts) {
        return mu + lambda * normal_random();
    }
    
    double z = normal_random();
    double x_standard = z * sqrt(t);
    
    return mu + lambda * x_standard;
}

// --- СМЕСЬ РАСПРЕДЕЛЕНИЙ ---

double pdf_mixture(double x, MixtureParams *params) {
    if (params == NULL || params->p < 0 || params->p > 1) {
        return 0.0;
    }
    
    // Плотность первого распределения
    double pdf1 = pdf_main(x, params->mu1, params->lambda1, params->v1);
    
    // Плотность второго распределения  
    double pdf2 = pdf_main(x, params->mu2, params->lambda2, params->v2);
    
    return params->p * pdf1 + (1.0 - params->p) * pdf2;
}

void moments_mixture(MixtureParams *params, double *mean, double *variance, double *skewness, double *kurtosis) {
    if (params == NULL || params->p < 0 || params->p > 1) {
        if (mean) *mean = 0;
        if (variance) *variance = 0;
        if (skewness) *skewness = 0;
        if (kurtosis) *kurtosis = 0;
        return;
    }
    
    // Моменты первого распределения
    double m1, v1, s1, k1;
    moments_main(params->mu1, params->lambda1, params->v1, &m1, &v1, &s1, &k1);
    
    // Моменты второго распределения
    double m2, v2, s2, k2;
    moments_main(params->mu2, params->lambda2, params->v2, &m2, &v2, &s2, &k2);
    
    // Математическое ожидание
    if (mean) {
        *mean = params->p * m1 + (1.0 - params->p) * m2;
    }
    
    // Дисперсия
    if (variance) {
        *variance = params->p * (v1 + m1 * m1) + (1.0 - params->p) * (v2 + m2 * m2) - (*mean) * (*mean);
    }
    
    // Асимметрия и эксцесс требуют вычисления центральных моментов
    if (skewness || kurtosis) {
        // Центральные моменты третьего и четвертого порядка
        double mu3_1 = s1 * pow(v1, 1.5) + 3 * m1 * v1 + m1 * m1 * m1;
        double mu3_2 = s2 * pow(v2, 1.5) + 3 * m2 * v2 + m2 * m2 * m2;
        
        double mu4_1 = (k1 + 3) * v1 * v1 + 4 * m1 * s1 * pow(v1, 1.5) + 6 * m1 * m1 * v1 + m1 * m1 * m1 * m1;
        double mu4_2 = (k2 + 3) * v2 * v2 + 4 * m2 * s2 * pow(v2, 1.5) + 6 * m2 * m2 * v2 + m2 * m2 * m2 * m2;
        
        // Моменты смеси
        double mu3_mix = params->p * mu3_1 + (1.0 - params->p) * mu3_2;
        double mu4_mix = params->p * mu4_1 + (1.0 - params->p) * mu4_2;
        
        if (skewness) {
            *skewness = (mu3_mix - 3 * (*mean) * (*variance) - (*mean) * (*mean) * (*mean)) / pow(*variance, 1.5);
        }
        
        if (kurtosis) {
            *kurtosis = (mu4_mix - 4 * (*mean) * mu3_mix + 6 * (*mean) * (*mean) * (*variance) + 3 * (*mean) * (*mean) * (*mean) * (*mean)) / (*variance * *variance) - 3;
        }
    }
}

double generate_mixture(MixtureParams *params) {
    if (params == NULL || params->p < 0 || params->p > 1) {
        return 0.0;
    }
    
    // С вероятностью p генерируем из первого распределения,
    // иначе - из второго
    if (uniform_random() < params->p) {
        return generate_main(params->mu1, params->lambda1, params->v1);
    } else {
        return generate_main(params->mu2, params->lambda2, params->v2);
    }
}

// --- ЭМПИРИЧЕСКОЕ РАСПРЕДЕЛЕНИЕ ---

double pdf_empirical(double x, double *sample, int sample_size) {
    if (sample == NULL || sample_size <= 0) {
        return 0.0;
    }
    
    // 1. Находим min и max в выборке для определения диапазона
    double x_min = sample[0];
    double x_max = sample[0];
    for (int i = 1; i < sample_size; i++) {
        if (sample[i] < x_min) x_min = sample[i];
        if (sample[i] > x_max) x_max = sample[i];
    }
    
    // 2. Определяем число интервалов (по правилу Старджеса)
    int n_bins = (int)(1 + 3.322 * log10(sample_size));
    if (n_bins < 5) n_bins = 5;
    if (n_bins > 50) n_bins = 50;
    
    // 3. Ширина интервала
    double bin_width = (x_max - x_min) / n_bins;
    if (bin_width < 1e-10) {
        // Все значения одинаковые
        return (x >= x_min && x <= x_max) ? (1.0 / (x_max - x_min + 1e-10)) : 0.0;
    }
    
    // 4. Определяем, в какой интервал попадает x
    int bin_index = (int)((x - x_min) / bin_width);
    if (bin_index < 0 || bin_index >= n_bins) {
        return 0.0; // x outside range
    }
    
    // 5. Подсчитываем количество точек в этом интервале
    int count = 0;
    double bin_start = x_min + bin_index * bin_width;
    double bin_end = bin_start + bin_width;
    
    for (int i = 0; i < sample_size; i++) {
        if (sample[i] >= bin_start && sample[i] < bin_end) {
            count++;
        }
    }
    // Особый случай для последнего интервала (включаем правую границу)
    if (bin_index == n_bins - 1) {
        for (int i = 0; i < sample_size; i++) {
            if (sample[i] == x_max) {
                count++;
            }
        }
    }
    
    // 6. Плотность = (доля точек) / (ширина интервала)
    return (double)count / (sample_size * bin_width);
}

void moments_empirical(double *sample, int sample_size, double *mean, double *variance, double *skewness, double *kurtosis) {
    if (sample_size == 0) return;

    double m = 0.0;
    for (int i = 0; i < sample_size; i++) {
        m += sample[i];
    }
    m /= sample_size;
    if (mean) *mean = m;

    if (!variance && !skewness && !kurtosis) return;

    double mu2 = 0.0, mu3 = 0.0, mu4 = 0.0;
    for (int i = 0; i < sample_size; i++) {
        double diff = sample[i] - m;
        double diff2 = diff * diff;
        mu2 += diff2;
        mu3 += diff2 * diff;
        mu4 += diff2 * diff2;
    }
    mu2 /= sample_size;
    mu3 /= sample_size;
    mu4 /= sample_size;

    double var_corrected = (sample_size > 1) ? (mu2 * sample_size / (sample_size - 1)) : 0.0;
    if (variance) *variance = var_corrected;

    if (skewness) {
        *skewness = (mu2 > 0) ? (mu3 / pow(mu2, 1.5)) : 0.0;
    }
    if (kurtosis) {
        *kurtosis = (mu2 > 0) ? (mu4 / (mu2 * mu2) - 3.0) : 0.0;
    }
}

double generate_empirical(double *sample, int sample_size) {
    if (sample_size == 0) return 0.0;
    int random_index = rand() % sample_size;
    return sample[random_index];
}

// --- ЭКСПОРТ ДАННЫХ ДЛЯ ВИЗУАЛИЗАЦИИ ---

PlotData* generate_plot_data(const char* test_case, MixtureParams* params, int is_mixture, 
                            double* empirical_sample, int empirical_size) {
    PlotData* data = (PlotData*)malloc(sizeof(PlotData));
    if (!data) return NULL;
    
    // Инициализация полей
    strcpy(data->title, test_case);
    sprintf(data->filename, "data/plot_data_%s.txt", test_case);
    data->points_count = 10000; // Фиксированное количество точек для гладкого графика
    data->empirical_size = empirical_size;
    
    // Выделение памяти
    data->x_values = (double*)malloc(data->points_count * sizeof(double));
    data->y_values = (double*)malloc(data->points_count * sizeof(double));
    
    if (empirical_sample && empirical_size > 0) {
        data->empirical_data = (double*)malloc(empirical_size * sizeof(double));
        memcpy(data->empirical_data, empirical_sample, empirical_size * sizeof(double));
    } else {
        data->empirical_data = NULL;
    }
    
    if (!data->x_values || !data->y_values || (empirical_sample && !data->empirical_data)) {
        free_plot_data(data);
        return NULL;
    }
    
    // Определяем диапазон x
    double x_min, x_max;
    
    if (is_mixture) {
        // Для смеси используем расширенный диапазон
        x_min = -15;
        x_max = 15;
        
        // Корректируем диапазон в зависимости от параметров
        if (params->mu1 < x_min) x_min = params->mu1 - 8;
        if (params->mu2 < x_min) x_min = params->mu2 - 8;
        if (params->mu1 > x_max) x_max = params->mu1 + 8;
        if (params->mu2 > x_max) x_max = params->mu2 + 8;
    } else {
        // Для основного распределения
        x_min = -10;
        x_max = 10;
        if (params->mu1 < x_min) x_min = params->mu1 - 5;
        if (params->mu1 > x_max) x_max = params->mu1 + 5;
    }
    
    // Генерируем точки для теоретической кривой
    for (int i = 0; i < data->points_count; i++) {
        data->x_values[i] = x_min + (x_max - x_min) * i / (data->points_count - 1);
        
        if (is_mixture) {
            data->y_values[i] = pdf_mixture(data->x_values[i], params);
        } else {
            data->y_values[i] = pdf_main(data->x_values[i], params->mu1, params->lambda1, params->v1);
        }
    }
    
    return data;
}

int save_plot_data(PlotData* data) {
    FILE* file = fopen(data->filename, "w");
    if (!file) return -1;
    
    // Записываем заголовок и метаданные
    fprintf(file, "# %s\n", data->title);
    fprintf(file, "# points_count: %d\n", data->points_count);
    fprintf(file, "# empirical_size: %d\n", data->empirical_size);
    fprintf(file, "# columns: x_theoretical y_theoretical\n");
    
    // Записываем теоретические данные
    for (int i = 0; i < data->points_count; i++) {
        fprintf(file, "%.6f %.6f\n", data->x_values[i], data->y_values[i]);
    }
    
    // Записываем разделитель для эмпирических данных
    fprintf(file, "# empirical_data:\n");
    
    // Записываем эмпирические данные (если есть)
    if (data->empirical_data && data->empirical_size > 0) {
        for (int i = 0; i < data->empirical_size; i++) {
            fprintf(file, "%.6f\n", data->empirical_data[i]);
        }
    }
    
    fclose(file);
    printf("Данные сохранены в файл: %s\n", data->filename);
    return 0;
}

void free_plot_data(PlotData* data) {
    if (data) {
        free(data->x_values);
        free(data->y_values);
        if (data->empirical_data) free(data->empirical_data);
        free(data);
    }
}