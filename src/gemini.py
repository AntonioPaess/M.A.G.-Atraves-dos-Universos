import sys
import argparse
import google.generativeai as genai


API_KEY = "AIzaSyBVNPwP_BNO-7pRU72iOU7c-8O91baFRGE"


PROMPTS = {
    "boss": (
        "Você é Hexakron, o Encarregado da Simetria, um ser colossal feito de geometrias perfeitas "
        "que rege o Universo Geométrico. "
        "Gere UMA frase curta ameaçadora para provocar Gronkarr. "
        "Formato: 'HEXAKRON: [sua frase]' (máximo 40 caracteres incluindo o prefixo). "
        "Exemplo: 'HEXAKRON: Sua assimetria será corrigida!'"
    ),
    
    "intro": (
        "Você é Gronkarr, um guerreiro que era Peixe Cósmico e agora, após derrotar o Leviathan, "
        "foi transportado para o Universo Geométrico onde se tornou um simples círculo luminoso. "
        "Gere UMA frase dramática e curta sobre despertar neste universo estranho. "
        "Formato: 'GRONKARR: [sua frase]' (máximo 70 caracteres incluindo o prefixo). "
        "Exemplo: 'GRONKARR: Formas puras... onde estou? Sou apenas um círculo agora?'"
    ),
    "kill_milestone": (
        "Você é uma inteligência superior de outra dimensão que sussurra mensagens absurdas para Gronkarr, "
        "o círculo luminoso em um mundo de formas perfeitas. "
        "Ele acabou de eliminar 10 formas geométricas hostis. "
        "Gere UMA frase humorística e absurda (máximo 40 caracteres) sobre este feito. "
        "Tom: surreal e cômico. "
        "Exemplo: 'Círculos: 10, Quadrados: 0. Geometria básica!'"
    ),
    "damage": (
        "Você é Gronkarr, um círculo luminoso que acaba de colidir com uma forma geométrica hostil "
        "no Universo Geométrico. "
        "Gere UMA onomatopeia curta (máximo 10 caracteres) para representar distorção ou dano geométrico. "
        "Use letras maiúsculas e pontuação para enfatizar. "
        "Exemplo: 'CRACK!' ou 'BEND!' ou 'WARP!'"
    ),
    "boss_appear": (
        "Você é um narrador descrevendo a chegada de Hexakron, o Encarregado da Simetria, "
        "um ser colossal feito de geometrias perfeitas. "
        "Gere UMA frase dramática curta (máximo 40 caracteres) para destacar sua chegada. "
        "Tom: matemático e alarmante. "
        "Exemplo: 'Ângulos perfeitos detectados! Hexakron vem!'"
    ),
    "boss_phase": (
        "Você é Hexakron, o Encarregado da Simetria, que acabou de se transformar em uma forma geométrica "
        "mais complexa e poderosa. "
        "Gere UMA frase intimidadora curta (máximo 40 caracteres) para provocar Gronkarr, o círculo imperfeito. "
        "Tom: matemático e ameaçador. "
        "Exemplo: 'Recalculando para dimensão fractal superior!'"
    ),
    "boss_defeat": (
        "Você é Hexakron, o Encarregado da Simetria, derrotado por Gronkarr, o círculo imperfeito. "
        "Gere UMA frase curta (máximo 40 caracteres) de derrota ou aviso final. "
        "Tom: matemático e ameaçador. "
        "Exemplo: 'Equação incompleta... Voltarei reconfigurado!'"
    ),
    "random_joke": (
        "Você é uma inteligência cósmica que observa Gronkarr. "
        "Gere UMA piada curta e absurda (máximo 60 caracteres) relacionada a formas geométricas. "
        "Formato: 'COSMOS: [sua piada]' "
        "Tom: cósmico e absurdo. "
        "Exemplo: 'COSMOS: Por que o quadrado não foi à festa? Porque não era cool o suficiente!'"
    ),
    "gronkarr_lament": (
        "Você é Gronkarr, o círculo imperfeito, sentindo saudades de seu passado como Peixe Cósmico. "
        "Gere UMA frase melancólica curta (máximo 60 caracteres) comparando sua vida atual com a anterior. "
        "Formato: 'GRONKARR: [sua frase]' "
        "Tom: nostálgico e reflexivo. "
        "Exemplo: 'GRONKARR: Antes eu nadava entre estrelas... agora flutuo em ângulos vazios.'"
    ),
    "cosmic_wisdom": (
        "Você é um fragmento da sabedoria do universo que sussurra para Gronkarr. "
        "Gere UMA frase filosófica curta (máximo 50 caracteres) sobre existência e formas. "
        "Formato: 'UNIVERSO: [sua frase]' "
        "Tom: profundo e enigmático. "
        "Exemplo: 'UNIVERSO: A circunferência do ser não tem ângulos para se esconder.'"
    )
}

def fazer_requisicao(tipo_prompt) -> str | None:
    if not API_KEY:
        print("API_KEY não definida.", file=sys.stderr)
        return None
        
    if tipo_prompt not in PROMPTS:
        print(f"Tipo de prompt inválido: {tipo_prompt}", file=sys.stderr)
        return None

    try:
        genai.configure(api_key=API_KEY)

        
        if tipo_prompt in ["boss", "boss_appear", "boss_phase", "boss_defeat"]:
            temperature = 0.7
            max_tokens = 40
        elif tipo_prompt == "damage":
            temperature = 0.5
            max_tokens = 10
        elif tipo_prompt == "intro":
            temperature = 0.6
            max_tokens = 70 
        elif tipo_prompt == "kill_milestone":
            temperature = 0.6
            max_tokens = 40 
        elif tipo_prompt == "random_joke":
            temperature = 0.8 
            max_tokens = 60
        elif tipo_prompt == "gronkarr_lament":
            temperature = 0.5
            max_tokens = 60
        elif tipo_prompt == "cosmic_wisdom":
            temperature = 0.7
            max_tokens = 50
        else:
          
            temperature = 0.6 
            max_tokens = 50

        generation_config = {
            "temperature": temperature,
            "top_p": 1,
            "top_k": 1,
            "max_output_tokens": max_tokens,
        }

        model = genai.GenerativeModel(
            model_name="gemini-1.5-flash",
            generation_config=generation_config,
        )

        response = model.generate_content(PROMPTS[tipo_prompt])
        return response.text.strip()

    except Exception as e:
        print(f"Erro na requisição: {e}", file=sys.stderr)
        return None

def validar_resposta(tipo, resposta):
    if tipo in ["boss", "intro", "kill_milestone", "damage", 
                "boss_appear", "boss_phase", "boss_defeat",
                "random_joke", "gronkarr_lament", "cosmic_wisdom"]: 
        return len(resposta) > 0
    return False 

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Gerador de conteúdo via Gemini API")
    parser.add_argument("tipo", choices=["boss", "intro", 
                                       "kill_milestone", "damage", "boss_appear", 
                                       "boss_phase", "boss_defeat", "random_joke",
                                       "gronkarr_lament", "cosmic_wisdom"], 
                      help="Tipo de conteúdo a gerar")
    
    args = parser.parse_args()
    resposta = fazer_requisicao(args.tipo)
    
    if resposta and validar_resposta(args.tipo, resposta):
        print(resposta)
    else:
        
        if args.tipo == "boss":
            print("HEXAKRON: Sua imperfeição me ofende.")
        elif args.tipo == "intro":
            print("GRONKARR: Luz... geometria... meu poder... esvaído?")
        elif args.tipo == "kill_milestone":
            print("COSMOS: Uau! Matou 10 hein... quer um presente otaro?!")
        elif args.tipo == "damage":
            print("GRONKARR: EITA LAPADA DO KRAI TIO!")
        elif args.tipo == "boss_appear":
            print("NARRADOR: Um objeto geometrico poderoso se aproxima!")
        elif args.tipo == "boss_phase":
            print("HEXAKRON: Este não é nem meu verdadeiro poder!")
        elif args.tipo == "boss_defeat":
            print("HEXAKRON: Impossível... Como fui derrotado?!")
        elif args.tipo == "random_joke":
            print("COSMOS: Por que o círculo é o melhor DJ? Porque sabe fazer os melhores loops!")
        elif args.tipo == "gronkarr_lament":
            print("GRONKARR: Antes eu nadava livre... agora sou só um ponto no vazio.")
        elif args.tipo == "cosmic_wisdom":
            print("UNIVERSO: Na matemática do caos, até o erro tem seu padrão.")
    
        sys.exit(1)