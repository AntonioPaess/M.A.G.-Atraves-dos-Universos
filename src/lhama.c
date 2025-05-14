// #include "llama.h"
// #include <curl/curl.h>
// #include <stdlib.h>
// #include <string.h>

// // Estrutura auxiliar para receber resposta da API
// struct MemoryStruct {
//     char *memory;
//     size_t size;
// };

// static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
//     size_t realsize = size * nmemb;
//     struct MemoryStruct *mem = (struct MemoryStruct *)userp;
//     char *ptr = realloc(mem->memory, mem->size + realsize + 1);
//     if(ptr == NULL) return 0; // falha na alocação
//     mem->memory = ptr;
//     memcpy(&(mem->memory[mem->size]), contents, realsize);
//     mem->size += realsize;
//     mem->memory[mem->size] = 0;
//     return realsize;
// }

// static char* callReplicateAPI(const char *model, const char *prompt) {
//     CURL *curl;
//     CURLcode res;
//     struct curl_slist *headers = NULL;
//     struct MemoryStruct chunk;
//     chunk.memory = malloc(1);
//     chunk.size = 0;

//     curl = curl_easy_init();
//     if (!curl) return NULL;

//     // URL de predição para o modelo específico (ex.: llama-2-7b-chat)
//     curl_easy_setopt(curl, CURLOPT_URL, "https://api.replicate.com/v1/predictions");
//     curl_easy_setopt(curl, CURLOPT_POST, 1L);

//     // Corpo JSON da requisição (ajuste conforme documentação do modelo)
//     const char *dataTemplate = "{\\\"version\\\": \\\"Evolua_para_a_versão\\\", \\\"input\\\": { \\\"prompt\\\": \\\"%s\\\" } }";
//     // Substitua "Evolua_para_a_versão" pelo ID correto da versão do modelo Llama 2 no Replicate.
//     char data[1024];
//     snprintf(data, sizeof(data), dataTemplate, prompt);

//     // Cabeçalhos
//     headers = curl_slist_append(headers, "Content-Type: application/json");
//     char authHeader[256];
//     snprintf(authHeader, sizeof(authHeader), "Authorization: Token %s", REPLICATE_API_TOKEN);
//     headers = curl_slist_append(headers, authHeader);
//     curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

//     curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
//     curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
//     curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

//     // Executa a requisição
//     res = curl_easy_perform(curl);

//     if (res != CURLE_OK) {
//         fprintf(stderr, "Erro curl: %s\n", curl_easy_strerror(res));
//         curl_easy_cleanup(curl);
//         free(chunk.memory);
//         return NULL;
//     }

//     curl_easy_cleanup(curl);
//     curl_slist_free_all(headers);

//     // Na resposta, parse para extrair o texto. Aqui assumimos que o texto completo vem em "output".
//     // Para simplificação, retornamos todo o JSON recebido (em um caso real deveríamos parsear JSON).
//     return chunk.memory;
// }

// char* generatePhaseName(int level) {
//     char prompt[256];
//     snprintf(prompt, sizeof(prompt), "Crie um nome engraçado para a fase %d do jogo MAG, que é um shooter top-down com gorilas inimigos.", level);
//     return callReplicateAPI("meta/llama-2-7b-chat", prompt);
// }

// char* generateDialogue(const char *prompt) {
//     // Reutiliza callReplicateAPI com prompt customizado
//     return callReplicateAPI("meta/llama-2-7b-chat", prompt);
// }
