#pragma once

#ifndef MAIN_APP_H_
#define MAIN_APP_H_

// Dzięki sekcji 'extern "C"' kompilator C++ wygeneruje symbole funkcji w taki sposób,
// aby plik main.c (kompilowany jako czyste C) mógł ją bez problemu wywołać.
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Główna funkcja aplikacji C++, wywoływana z poziomu main.c
 * zaraz przed nieskończoną pętlą while(1).
 */
void cpp_main(void);

#ifdef __cplusplus
}
#endif

#endif /* MAIN_APP_H_ */