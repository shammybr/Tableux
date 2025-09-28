// Logica.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <io.h>     // Necessário para _setmode
#include <fcntl.h>  // Necessário para _O_U16TEXT
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>

enum EConectivo {
    ENADA = 0,
    ENAO = 1,
    EIMPLICA = 2,
    EOU = 3,
    EE = 4,


};

bool ChecaValidez(std::wstring expression, std::wstring* erro);
std::wstring LerExpressao();
bool IsTautologia(std::vector <std::wstring> vBD, std::wstring pergunta);
std::vector <std::wstring> AplicarRegra(std::wstring expressao, int regra);
void ResolverGalho(std::vector <std::wstring>* galho, std::vector <std::vector <std::wstring>>* arvore);
int TirarColchetesFora(std::wstring* expressao);
EConectivo ParsearExpressao(std::wstring expressao, int* indice);
bool ResolverExpressao(std::wstring expressao, int indice, EConectivo conectivo, std::vector <std::wstring> *galho, std::vector <std::vector <std::wstring>> * arvore);

enum eEstadoAtual {
    ESTEP1,
    ESTEP2,
    ESTEP3,
    ESTEP4,
};

int main(){


    _setmode(_fileno(stdout), _O_U16TEXT);

    std::wcout << "Glossário de expressões:\n";
    std::wcout << "Letra maiúscula = Variável (A, B, C,..., Z)\n";
    std::wcout << "~(Expressão) = Não (Expressão)\n";
    std::wcout << "(A > B) = A implica B\n";
    std::wcout << "(A v B) = A ou B\n";
    std::wcout << "(A ^ B) = A e B\n\n";
    

    std::wstring expression;

    std::wstring expressionNoSpaces;

    std::vector <std::wstring> vBD;
    std::wstring pergunta = L"";

    do {
        vBD.clear();
        pergunta.clear();

        do {

           

            std::wcout << "\nDigite as expressões do BD (vazio para continuar):\n";
            expressionNoSpaces.clear();
            expressionNoSpaces = LerExpressao();

            /*  std::getline(std::wcin, expression);
              std::wcout << expression.c_str() << " " << expression.size() << std::endl;

              for (int i = 0; i < expression.size(); i++) {
                  if (expression[i] != ' ') {
                      expressionNoSpaces += expression[i];
                  }
              }

              std::wcout << expressionNoSpaces.c_str() << " " << expressionNoSpaces.size() << std::endl;
              */
            std::wstring erro = L"";
            std::wstring* erroPtr = &erro;


            if (ChecaValidez(expressionNoSpaces, erroPtr)) {
                std::wcout << expressionNoSpaces.c_str() << " adicionado ao BD!" << std::endl;
                vBD.insert(vBD.end(), expressionNoSpaces);
            }
            else {
                if (erro.size() > 0) {
                    std::wcout << erro << std::endl;
                }
            }

        } while (vBD.size() == 0 || expressionNoSpaces != L"");

        std::wcout << "\nBD: \n";
        for (int i = 0; i < vBD.size(); i++) {
            std::wcout << vBD[i].c_str() << " " << std::endl;

        }

        std::wcout << "\nDigite a pergunta:\n";


        do {
            expressionNoSpaces = LerExpressao();
            std::wstring erro = L"";
            std::wstring* erroPtr = &erro;


            if (ChecaValidez(expressionNoSpaces, erroPtr)) {
                pergunta = expressionNoSpaces;
            }
            else {
                if (erro.size() > 0) {
                    std::wcout << erro << std::endl;
                }
            }
        } while (pergunta == L"");




        //std::vector <std::wstring> regraAplicada = AplicarRegra(pergunta, 6);



        //for (int i = 0; i < regraAplicada.size(); i++) {
        //    for (int k = 0; k < vBD.size(); k++) {
        //        std::wcout << regraAplicada[i].c_str() << " " << std::endl;

        //    }

        //}



        IsTautologia(vBD, pergunta);
    } while (true);
       
}





std::wstring LerExpressao() {

    std::wstring expression;
    std::wstring expressionNoSpaces;
    std::getline(std::wcin, expression);

    for (int i = 0; i < expression.size(); i++) {
        if (expression[i] != ' ') {
            expressionNoSpaces += expression[i];
        }
    }

    return expressionNoSpaces;
}

bool ChecaValidez(std::wstring expression, std::wstring* erro) {
    int step = ESTEP1;
    int colchetesAbertos = 0;

    // A -Z >= 65  <= 90
    //  ~ == 126  
    //  v ^ > == 118  == 94 == 94  == 62
    if (expression.size() == 0) {
        return false;
    }

    for (int i = 0; i < expression.size(); i++) {
        switch (step) {
            //   (  A-Z  ~
            case ESTEP1:

                if (expression[i] == '(') {
                    colchetesAbertos++;
                }
                else if (expression[i] >= 65 && expression[i] <= 90) {
                    step = ESTEP2;
                }
                else if (expression[i] == 126) {
                    step = ESTEP3;
                }
                else {
                    *erro = std::wstring(L"Formato incorreto! Erro em (") + expression[i] + L")";
                    return false;
                }
                break;

                //  ) v ^ > 
            case ESTEP2:
                if (expression[i] == ')') {
                    if (colchetesAbertos > 0) {
                        colchetesAbertos--;
                        step = ESTEP4;
                    }
                    else {
                        *erro = std::wstring(L"Formato incorreto! Fechando colchete não aberto!");
                    }
                }

                else if (expression[i] == 118 || expression[i] == 94 || expression[i] == 62) {
                    step = ESTEP3;
                }
                else {
                    *erro = std::wstring(L"Operador esperado, encontrado (") + expression[expression.size() - 1] + L")";
                    return false;
                }

                break;
                // ( A-Z ~
            case ESTEP3:
                if (expression[i] == '(') {
                    step = ESTEP1;
                    colchetesAbertos++;
                }
                else if (expression[i] == 126) {
                    //step = ESTEP3;
                }
                else if (expression[i] >= 65 && expression[i] <= 90) {
                    step = ESTEP4;
                }
                else {
                    *erro = std::wstring(L"Formato incorreto! Erro em (") + expression[i] + L")";
                    return false;
                }

                break;

                //  )  v ^ > 
            case ESTEP4:

                if (expression[i] == ')') {
                    if (colchetesAbertos > 0) {
                        colchetesAbertos--;
                    }
                    else {
                        *erro = std::wstring(L"Formato incorreto! Fechando colchete não aberto!");
                        return false;
                    }
                }
                else if (expression[i] == 118 || expression[i] == 94 || expression[i] == 62) {
                    step = ESTEP3;
                }
                else {
                    *erro = std::wstring(L"Formato incorreto! Erro em (") + expression[i] + L")";
                    return false;
                }

                break;




        }


    }

    if (colchetesAbertos == 0) {
       // std::wcout << "Step:       " << step << std::endl;
        if (step == ESTEP2 && expression.size() != 1) {
            *erro = std::wstring(L"Operador esperado, encontrado (") + expression[expression.size() - 1] + L")";
            return false;
        }
        else if (step == ESTEP3) {
            *erro = std::wstring(L"Formato incorreto! Erro em (") + expression[expression.size() - 1] + L")";
            return false;
        }
            return true;
    }
    else {
        *erro = std::wstring(L"Formato incorreto! Erro em ") + std::to_wstring(colchetesAbertos) + L" colchetes não fechados.";
        return false;
    }
}

bool IsTautologia(std::vector <std::wstring> vBD, std::wstring pergunta) {

    pergunta.insert(pergunta.begin(), '(');
    pergunta.insert(pergunta.end(), ')');
    pergunta.insert(pergunta.begin(), '~');


    std::vector <std::vector <std::wstring>> arvore;
    std::vector <std::wstring> galho;
    for (int i = 0; i < vBD.size(); i++) {
        galho.insert(galho.end(), vBD[i]);
    }

    galho.insert(galho.end(), pergunta);

    arvore.insert(arvore.end(), galho);

    int galhosResolvidos = 0;


    bool arvoreResolvida = false;

    while (!arvoreResolvida) {
        arvoreResolvida = true;
        int galhoI = 0;

        for (int a = 0; ; ) {
            bool galhoResolvido = false;

            std::wcout << std::endl <<  "Galho atual:  " << a + 1 << std::endl;
            while (!galhoResolvido) {
                galhoResolvido = true;



                for (int i = 0; i < arvore[a].size(); i++) {
                    int indice = -1;

                    EConectivo conectivo = ParsearExpressao(arvore[a][i], &indice);


                    //std::wcout << "conectivo:  " << conectivo << " Indice: " << indice << std::endl;

                    if (ResolverExpressao(arvore[a][i], indice, conectivo, &arvore[a], &arvore)) {
                        galhoResolvido = false;
                        arvoreResolvida = false;
                        break;
                    }
                }

            }

            a++;
            if (a >= arvore.size()) {
                    break;
            }

        }
    }

    std::vector <bool> contradicoes;

    for (int i = 0; i < arvore.size(); i++) {
        bool contradicao = false;

        for (int k = 0; k < arvore[i].size(); k++) {
            for (int l = 0; l < arvore[i].size(); l++) {
                std::wstring contrario = L"~";
                contrario += arvore[i][l];

                if (arvore[i][k] == contrario) {
                    contradicao = true;
                    break;
                }
            }

        }

        contradicoes.insert(contradicoes.end(), contradicao);

    }

    for (bool contradicao : contradicoes) {
        if (contradicao == false) {
            std::wcout << std::endl << "Não é tautologia!!!!" << std::endl << std::endl;
            return false;
        }

    }

    std::wcout << std::endl << "É tautologia!!!!" << std::endl << std::endl;
    return true;
}

EConectivo ParsearExpressao(std::wstring expressao, int *indice) {

    indice += TirarColchetesFora(&expressao);

    int colchetesAbertos = 0;
    int regra = 0;
    EConectivo conectivo = ENADA;

    for (int i = 0; i < expressao.size(); i++) {

        if (expressao[i] == L'(') {
            colchetesAbertos++;
        }
        else if (expressao[i] == L')') {
            colchetesAbertos--;
        }
        else if (colchetesAbertos == 0) {
            if (expressao[i] == L'~') {
                if (conectivo == ENADA) {
                    conectivo = ENAO;
                    *indice = i;
                }

            }
            else if (expressao[i] == L'>') {
                if (conectivo <= EIMPLICA) {
                    conectivo = EIMPLICA;
                    *indice = i;
                }
            }
            else if (expressao[i] == L'v') {
                if (conectivo <= EOU) {
                    conectivo = EOU;
                    *indice = i;
                }

            }
            else if (expressao[i] == L'^') {
                    conectivo = EE;
                    *indice = i;

            }


        }


    }

    return conectivo;

}

bool ResolverExpressao(std::wstring expressao, int indice, EConectivo conectivo, std::vector <std::wstring> *galho, std::vector <std::vector <std::wstring>> *arvore) {

    std::wstring expressaoFinal;
    std::vector  <std::wstring> expressaoARemover;
    std::vector  <std::wstring> expressaoAAdicionar;
    std::wstring expressaoAAdicionarNovoGalho;


    switch (conectivo) {
        case ENAO: {
            std::wstring subExpressao;

            for (int i = indice + 1; i < expressao.size(); i++) {
                subExpressao += expressao[i];

            }

            if (subExpressao.size() > 1) {
                TirarColchetesFora(&subExpressao);

                int subIndice = 0;
                EConectivo subConectivo = ParsearExpressao(subExpressao, &subIndice);

                switch (subConectivo) {
                    case ENADA: {

                        subExpressao.insert(subExpressao.begin(), L'~');
                        expressaoAAdicionar.insert(expressaoAAdicionar.end(), subExpressao);
                        expressaoARemover.insert(expressaoARemover.end(), expressao);

                        std::wcout << "Retirando Colchetes de " << expressao << std::endl;
                    }
                              break;
                    case ENAO: {
                        std::vector <std::wstring> regraAplicada = AplicarRegra(expressao, 4);

                        for (std::wstring novaExpressao : regraAplicada) {
                            TirarColchetesFora(&novaExpressao);
                            expressaoAAdicionar.insert(expressaoAAdicionar.end(), novaExpressao);
                        }

                        expressaoARemover.insert(expressaoARemover.end(), expressao);

                        std::wcout << "Aplicando regra 4 em " << expressao << std::endl;
                    }
                             break;

                    case EIMPLICA: {
                        std::vector <std::wstring> regraAplicada = AplicarRegra(expressao, 7);


                        for (std::wstring novaExpressao : regraAplicada) {
                            TirarColchetesFora(&novaExpressao);
                            expressaoAAdicionar.insert(expressaoAAdicionar.end(), novaExpressao);
                        }

                        expressaoARemover.insert(expressaoARemover.end(), expressao);

                        std::wcout << "Aplicando regra 7 em " << expressao << std::endl;
                    }
                                 break;

                    case EOU: {
                        std::vector <std::wstring> regraAplicada = AplicarRegra(expressao, 6);

                        for (std::wstring novaExpressao : regraAplicada) {
                            TirarColchetesFora(&novaExpressao);
                            expressaoAAdicionar.insert(expressaoAAdicionar.end(), novaExpressao);
                        }

                        expressaoARemover.insert(expressaoARemover.end(), expressao);

                        std::wcout << "Aplicando regra 6 em " << expressao << std::endl;
                    }
                            break;

                    case EE: {
                        std::vector <std::wstring> regraAplicada = AplicarRegra(expressao, 5);


                        TirarColchetesFora(&regraAplicada[0]);
                        expressaoAAdicionar.insert(expressaoAAdicionar.end(), regraAplicada[0]);
                        expressaoARemover.insert(expressaoARemover.end(), expressao);

                        TirarColchetesFora(&regraAplicada[1]);
                        expressaoAAdicionarNovoGalho = regraAplicada[1];

                        std::wcout << "Aplicando regra 5 em " << expressao << std::endl;

                    }

                           break;
                    }

            }
        }
                 break;

        case EIMPLICA: {
            std::vector <std::wstring> regraAplicada = AplicarRegra(expressao, 3);

            TirarColchetesFora(&regraAplicada[0]);
            expressaoAAdicionar.insert(expressaoAAdicionar.end(), regraAplicada[0]);
            expressaoARemover.insert(expressaoARemover.end(), expressao);

            TirarColchetesFora(&regraAplicada[1]);
            expressaoAAdicionarNovoGalho = regraAplicada[1];
 
            std::wcout << "Aplicando regra 3 em " << expressao << std::endl;
        }
            break;



        case EOU: {
            std::vector <std::wstring> regraAplicada = AplicarRegra(expressao, 2);

            TirarColchetesFora(&regraAplicada[0]);
            expressaoAAdicionar.insert(expressaoAAdicionar.end(), regraAplicada[0]);
            expressaoARemover.insert(expressaoARemover.end(), expressao);

            TirarColchetesFora(&regraAplicada[1]);
            expressaoAAdicionarNovoGalho = regraAplicada[1];

            std::wcout << "Aplicando regra 2 em " << expressao << std::endl;
        }
            break;


        case EE: {

            std::vector <std::wstring> regraAplicada = AplicarRegra(expressao, 1);

            for (std::wstring novaExpressao : regraAplicada) {
                TirarColchetesFora(&novaExpressao);
                expressaoAAdicionar.insert(expressaoAAdicionar.end(), novaExpressao);
            }

            expressaoARemover.insert(expressaoARemover.end(), expressao);

            std::wcout << "Aplicando regra 1 em " << expressao << std::endl;

        }

            break;

    }

    if (expressaoAAdicionar.size() > 0) {

        for (std::wstring novaExpressao : expressaoAAdicionar) {
            galho->insert(galho->end(), novaExpressao);
        }
        for (std::wstring paraRemover : expressaoARemover) {
            std::erase((*galho), paraRemover);
        }

        std::wcout << " ------- " << std::endl;
        for (std::wstring expressao : *galho) {

            std::wcout << expressao.c_str() << " " << std::endl;

        }
        std::wcout << " ------- " << std::endl;
        
        if (expressaoAAdicionarNovoGalho.size() > 0) {

            std::vector <std::wstring> novoGalho = *galho;


            novoGalho.insert(novoGalho.end(), expressaoAAdicionarNovoGalho);
            arvore->insert(arvore->end(), novoGalho);
        }

        return true;
    }


    return false;
}

std::vector <std::wstring> AplicarRegra(std::wstring expressao, int regra) {
    std::wstring alfa;
    std::wstring beta;
    std::vector <std::wstring> expressaoFinal;



    switch (regra) {
    case 1: {
        TirarColchetesFora(&expressao);


        int colchetesAbertos = 0;

        for (int i = 0; i < expressao.size(); i++) {
            if (expressao[i] == L'(') {
                colchetesAbertos++;
            }
            else if (expressao[i] == L')') {
                colchetesAbertos--;
            }
            else if (colchetesAbertos == 0) {
                if (expressao[i] == '^') {
                    for (int k = 0; k < expressao.size(); k++) {
                        if (k < i) {
                            alfa += expressao[k];
                        }
                        else if (k > i) {
                            beta += expressao[k];
                        }
                    }

                    int colchetesAbertos = 0;
                    for (int k = 0; k < alfa.size(); k++) {
                        if (alfa[i] == '(') {
                            colchetesAbertos++;
                        }
                        else  if (alfa[i] == ')') {
                            colchetesAbertos--;
                        }
                    }

                    while (colchetesAbertos != 0) {
                        if (colchetesAbertos > 0) {

                            alfa.erase(alfa.begin() + alfa.find_first_of('('));
                            colchetesAbertos--;
                        }
                        else if (colchetesAbertos < 0) {

                            alfa.erase(alfa.begin() + alfa.find_last_of(')'));
                            colchetesAbertos++;
                        }

                    }

                    expressaoFinal.insert(expressaoFinal.end(), alfa);
                    expressaoFinal.insert(expressaoFinal.end(), beta);

                    break;
                }
            }
        }

        break;
    }
    case 2: {
        TirarColchetesFora(&expressao);


        int colchetesAbertos = 0;

        for (int i = 0; i < expressao.size(); i++) {
            if (expressao[i] == L'(') {
                colchetesAbertos++;
            }
            else if (expressao[i] == L')') {
                colchetesAbertos--;
            }
            else if (colchetesAbertos == 0) {
                if (expressao[i] == 'v') {
                    for (int k = 0; k < expressao.size(); k++) {
                        if (k < i) {
                            alfa += expressao[k];
                        }
                        else if (k > i) {
                            beta += expressao[k];
                        }
                    }

                    int colchetesAbertos = 0;
                    for (int k = 0; k < alfa.size(); k++) {
                        if (alfa[i] == '(') {
                            colchetesAbertos++;
                        }
                        else  if (alfa[i] == ')') {
                            colchetesAbertos--;
                        }
                    }

                    while (colchetesAbertos != 0) {
                        if (colchetesAbertos > 0) {

                            alfa.erase(alfa.begin() + alfa.find_first_of('('));
                            colchetesAbertos--;
                        }
                        else if (colchetesAbertos < 0) {

                            alfa.erase(alfa.begin() + alfa.find_last_of(')'));
                            colchetesAbertos++;
                        }

                    }

                    expressaoFinal.insert(expressaoFinal.end(), alfa);
                    expressaoFinal.insert(expressaoFinal.end(), beta);

                    break;
                }
            }
        }

        break;
    }
    case 3: {
        {
            TirarColchetesFora(&expressao);

            int colchetesAbertos = 0;

            for (int i = 0; i < expressao.size(); i++) {
                if (expressao[i] == L'(') {
                    colchetesAbertos++;
                }
                else if (expressao[i] == L')') {
                    colchetesAbertos--;
                }
                else if (colchetesAbertos == 0) {
                    if (expressao[i] == '>') {
                        for (int k = 0; k < expressao.size(); k++) {
                            if (k < i) {
                                alfa += expressao[k];
                            }
                            else if (k > i) {
                                beta += expressao[k];
                            }
                        }

                        int colchetesAbertos = 0;
                        for (int k = 0; k < alfa.size(); k++) {
                            if (alfa[i] == '(') {
                                colchetesAbertos++;
                            }
                            else  if (alfa[i] == ')') {
                                colchetesAbertos--;
                            }
                        }

                        while (colchetesAbertos != 0) {
                            if (colchetesAbertos > 0) {

                                alfa.erase(alfa.begin() + alfa.find_first_of('('));
                                colchetesAbertos--;
                            }
                            else if (colchetesAbertos < 0) {

                                alfa.erase(alfa.begin() + alfa.find_last_of(')'));
                                colchetesAbertos++;
                            }

                        }

                        alfa.insert(alfa.begin(), '~');
                        expressaoFinal.insert(expressaoFinal.end(), alfa);
                        expressaoFinal.insert(expressaoFinal.end(), beta);

                        break;
                    }
                }
            }

            break;
        }

        break;
    }
    case 4: {

        int nots = 0;
        for (int i = 0; i < expressao.size(); i++) {
            if (expressao[i] == '~' && nots < 2) {
                nots++;
            }
            else {
                alfa += expressao[i];

            }
        }

        expressaoFinal.insert(expressaoFinal.end(), alfa);

        break;

    }
    case 5: {
        int noti = expressao.find_first_of('~');

        // ((~(A^B)))

        std::wstring subExpressao;

        for (int i = noti + 1; i < expressao.size(); i++) {
            subExpressao += expressao[i];
        }

        TirarColchetesFora(&subExpressao);
        int colchetesAbertos = 0;

        for (int i = 0; i < subExpressao.size(); i++) {



            if (subExpressao[i] == L'(') {
                colchetesAbertos++;
            }
            else if (subExpressao[i] == L')') {
                colchetesAbertos--;
            }
            else if (colchetesAbertos == 0) {



                if (subExpressao[i] == '^') {
                    for (int k = noti; k < subExpressao.size(); k++) {
                        if (k < i) {
                            alfa += subExpressao[k];
                        }
                        else if (k > i) {
                            beta += subExpressao[k];
                        }
                    }




                    alfa.insert(alfa.begin(), '~');
                    beta.insert(beta.begin(), '~');
                    expressaoFinal.insert(expressaoFinal.end(), alfa);
                    expressaoFinal.insert(expressaoFinal.end(), beta);

                    break;
                }
            }

        }

        break;

    }
    case 6: {
        int noti = expressao.find_first_of('~');

        // ((~(AvB)))

        std::wstring subExpressao;

        for (int i = noti + 1; i < expressao.size(); i++) {
            subExpressao += expressao[i];
        }

        TirarColchetesFora(&subExpressao);
        int colchetesAbertos = 0;

        for (int i = 0; i < subExpressao.size(); i++) {

            

            if (subExpressao[i] == L'(') {
                colchetesAbertos++;
            }
            else if (subExpressao[i] == L')') {
                colchetesAbertos--;
            }
            else if (colchetesAbertos == 0) {



                if (subExpressao[i] == 'v') {
                    for (int k = noti; k < subExpressao.size(); k++) {
                        if (k < i) {
                            alfa += subExpressao[k];
                        }
                        else if (k > i) {
                            beta += subExpressao[k];
                        }
                    }




                    alfa.insert(alfa.begin(), '~');
                    beta.insert(beta.begin(), '~');
                    expressaoFinal.insert(expressaoFinal.end(), alfa);
                    expressaoFinal.insert(expressaoFinal.end(), beta);

                    break;
                }
            }

        }

        break;

    }
    case 7:   int noti = expressao.find_first_of('~');

        // ((~(A>B)))

        std::wstring subExpressao;

        for (int i = noti + 1; i < expressao.size(); i++) {
            subExpressao += expressao[i];
        }

        TirarColchetesFora(&subExpressao);
        int colchetesAbertos = 0;

        for (int i = 0; i < subExpressao.size(); i++) {



            if (subExpressao[i] == L'(') {
                colchetesAbertos++;
            }
            else if (subExpressao[i] == L')') {
                colchetesAbertos--;
            }
            else if (colchetesAbertos == 0) {



                if (subExpressao[i] == '>') {
                    for (int k = noti; k < subExpressao.size(); k++) {
                        if (k < i) {
                            alfa += subExpressao[k];
                        }
                        else if (k > i) {
                            beta += subExpressao[k];
                        }
                    }




                    beta.insert(beta.begin(), '~');
                    expressaoFinal.insert(expressaoFinal.end(), alfa);
                    expressaoFinal.insert(expressaoFinal.end(), beta);

                    break;
                }
            }

        }

        break;

    }


    return expressaoFinal;
}























//LIXO




std::wstring MenorExpressaoANegar(std::wstring expressao, int *indice) {

    std::wstring expressaoANegar;


    int colchetesNegados = 0;

    int colchetesTirados = TirarColchetesFora(&expressao);

    if (expressao.size() > 1) {
        for (int i = 0; i < expressao.size(); i++) {
            if (expressao[i] == L'~') {
                if (expressao[i + 1] == L'(' or expressao[i + 1] == L'~') {
                    for (int k = i + 1; k < expressao.size(); k++) {
                        if (expressao[k] == L'(') {
                            colchetesNegados++;
                        }
                        else if (expressao[k] == L')') {
                            colchetesNegados--;
                        }

                        expressaoANegar += expressao[k];

                        if (colchetesNegados == 0) {
                            break;
                        }


                    }

                    int colchetesTirados2 = TirarColchetesFora(&expressaoANegar);

                    if (expressaoANegar.size() > 1) {
                        *indice += colchetesTirados + colchetesTirados2;
                        *indice += i;
                        return MenorExpressaoANegar(expressaoANegar, indice);
                    }
                }
            }


        }

    }
     return expressao;
   
}

int TirarColchetesFora(std::wstring *expressao) {

    bool colchetesFora = true;
    int colchetesTirados = 0;

    while ((*expressao)[0] == L'(' && (*expressao)[(*expressao).size() - 1] == L')' && colchetesFora) {
        int colchetesAbertos = 0;
        colchetesFora = true;
        for (int i = 0; i < (*expressao).size() - 1; i++) {
            if ((*expressao)[i] == L'(') {
                colchetesAbertos++;
            }
            else if ((*expressao)[i] == L')') {
                colchetesAbertos--;
            }


            if (colchetesAbertos == 0) {
                colchetesFora = false;
            }
        }

        if (colchetesFora) {
            (*expressao).erase((*expressao).begin());
            (*expressao).erase((*expressao).begin() + ((*expressao).size() - 1));
            colchetesTirados++;
        }

    }

    return colchetesTirados;
}

std::wstring AplicarNoMeio(std::wstring menorExpressaoANegar, std::wstring expressao, std::wstring expressaoANegar, int i, int indice, int regra) {

    std::vector <std::wstring> expressaoNegada = AplicarRegra(menorExpressaoANegar, regra);

    std::wstring expressaoFinal;

    bool foiNegada = false;
    for (int k = 0; k < expressao.size(); k++) {
        if (k < i) {
            expressaoFinal += expressao[k];
        }
        else {
            if (k < (indice + expressaoANegar.size())) {
                if (!foiNegada) {
                    for (auto caractere : expressaoNegada) {
                        expressaoFinal += caractere;
                    }
                    foiNegada = true;
                }
            }
            else {
                expressaoFinal += expressao[k];
            }
        }
    }


    TirarColchetesFora(&expressaoFinal);

    return expressaoFinal;
}

void ResolverGalho(std::vector <std::wstring> *galho, std::vector <std::vector <std::wstring>> *arvore) {
    bool isResolvido = false;

    //(AvB)^B
    //(AvB)v(AvB)^(A)
    //AvB
    //Av(A^B)vA
    //((A^B)^A)
    while (!isResolvido) {

        std::vector  <std::wstring> expressaoARemover;
        std::vector  <std::wstring> expressaoAAdicionar;

        for (std::wstring expressao : *galho) {

            std::wstring expressaoOriginal = expressao;

            TirarColchetesFora(&expressao);

            int colchetesAbertos = 0;
            if (expressao.size() > 2) {

                for (int i = 0; i < expressao.size(); i++) {

                    if (expressao[i] == L'(') {
                        colchetesAbertos++;
                    }
                    else if (expressao[i] == L')') {
                        colchetesAbertos--;
                    }
                    else if (colchetesAbertos == 0) {
                        // ~(~(AvB)vB) ~(~A) ~((~A))  v(AvB)  ~(A)vB
                        if (expressao[i] == '~') {

                            std::wstring expressaoANegar;


                            int colchetesNegados = 0;

                            if (expressao[i + 1] == '~') {
                                std::vector <std::wstring> regraAplicada = AplicarRegra(expressao, 4);

                                

                                for (std::wstring novaExpressao : regraAplicada) {
                                    TirarColchetesFora(&novaExpressao);
                                    expressaoAAdicionar.insert(expressaoAAdicionar.end(), novaExpressao);
                                }

                                expressaoARemover.insert(expressaoARemover.end(), expressaoOriginal);
                            }
                            else {
                                for (int k = i + 1; k < expressao.size(); k++) {
                                    if (expressao[k] == L'(') {
                                        colchetesNegados++;
                                    }
                                    else if (expressao[k] == L')') {
                                        colchetesNegados--;
                                    }
                                    expressaoANegar += expressao[k];

                                    if (colchetesNegados == 0) {
                                        break;
                                    }

                                }

                                int indice = i + 1;
                                std::wstring menorExpressaoANegar = expressaoANegar;
                                indice += TirarColchetesFora(&menorExpressaoANegar);

                                if (menorExpressaoANegar.size() > 1) {
                                    menorExpressaoANegar = MenorExpressaoANegar(expressaoANegar, &indice);


                                    std::wcout << menorExpressaoANegar.c_str() << " PARA NEGAR, INDICE  " << indice << std::endl;

                                    indice += TirarColchetesFora(&menorExpressaoANegar);

                                    int menorColchetesAbertos = 0;
                                    bool aplicou = false;
                                    for (int k = 0; k < menorExpressaoANegar.size(); k++) {
                                        if (menorExpressaoANegar[k] == L'(') {
                                            menorColchetesAbertos++;
                                        }
                                        else if (menorExpressaoANegar[k] == L')') {
                                            menorColchetesAbertos--;
                                        }

                                        else if (menorColchetesAbertos == 0) {
                                            if (menorExpressaoANegar[k] == L'v') {


                                                std::vector <std::wstring> expressaoNegada = AplicarRegra(menorExpressaoANegar, 6);
                                                // ~(~(AvB)^C)
                                                
                                                
                                                expressaoNegada.insert(expressaoNegada.begin() + 1, L"^");
                                                expressaoNegada.insert(expressaoNegada.begin(), L"(");
                                                expressaoNegada.insert(expressaoNegada.end(), L")");

                                                std::wstring expressaoFinal;

                                                bool foiNegada = false;
                                                for (int k = 0; k < expressao.size(); k++) {
                                                    if (k < (indice)) {
                                                        expressaoFinal += expressao[k];
                                                    }
                                                    else {
                                                        if (k < (indice + menorExpressaoANegar.size() + 1)) {
                                                          

                                                            if (!foiNegada) {
                                                                expressaoFinal.erase(expressaoFinal.find_last_of(L'~'));
                                                                for (auto caractere : expressaoNegada) {
                                                                    expressaoFinal += caractere;
                                                                }
                                                                foiNegada = true;
                                                            }
                                                        }
                                                        else {
                                                            expressaoFinal += expressao[k];
                                                        }
                                                    }
                                                }

                                                

                                                TirarColchetesFora(&expressaoFinal);

                                                std::vector <std::wstring> regraAplicada;
                                                regraAplicada.insert(regraAplicada.begin(), expressaoFinal);


                                                for (std::wstring novaExpressao : regraAplicada) {
                                                    expressaoAAdicionar.insert(expressaoAAdicionar.end(), novaExpressao);
                                                }

                                                expressaoARemover.insert(expressaoARemover.end(), expressaoOriginal);
                                                aplicou = true;

                                                break;
                                            }


                                        }

                                    }

                                    if (aplicou) {
                                        break;
                                    }


                                   // std::vector <std::wstring> expressaoNegada = AplicarRegra(menorExpressaoANegar, 4);

                                    std::wstring expressaoFinal = AplicarNoMeio(menorExpressaoANegar, expressao, expressaoANegar, i, indice, 4);

                                    std::vector <std::wstring> regraAplicada;
                                    regraAplicada.insert(regraAplicada.begin(), expressaoFinal);

                                    for (std::wstring novaExpressao : regraAplicada) {

                                        expressaoAAdicionar.insert(expressaoAAdicionar.end(), novaExpressao);
                                    }

                                    expressaoARemover.insert(expressaoARemover.end(), expressaoOriginal);

                                    break;
                                }
                                //if(expressaoANegar.size() > 2)
                            }
                        }
                        // ^
                        else if (expressao[i] == 94) {
                            std::vector <std::wstring> regraAplicada = AplicarRegra(expressao, 1);
                            for (std::wstring novaExpressao : regraAplicada) {
                                TirarColchetesFora(&novaExpressao);
                                expressaoAAdicionar.insert(expressaoAAdicionar.end(), novaExpressao);
                            }

                            expressaoARemover.insert(expressaoARemover.end(), expressaoOriginal);


                            break;
                        }
                        // >  A FAZER
                        else if (expressao[i] == 62) {
                            std::vector <std::wstring> regraAplicada = AplicarRegra(expressao, 3);
                            for (std::wstring novaExpressao : regraAplicada) {
                                TirarColchetesFora(&novaExpressao);
                                expressaoAAdicionar.insert(expressaoAAdicionar.end(), novaExpressao);
                            }

                            expressaoARemover.insert(expressaoARemover.end(), expressaoOriginal);
                        }


                    }
                }

            }

        }

        for (std::wstring novaExpressao : expressaoAAdicionar) {
            galho->insert(galho->end(), novaExpressao);
        }
        for (std::wstring paraRemover : expressaoARemover) {
            std::erase((*galho), paraRemover);
        }

      
        isResolvido = true;

        std::wcout << " ------- " << std::endl;
        for (std::wstring expressao : *galho) {

            std::wcout << expressao.c_str() << " " << std::endl;

            for (int i = 0; i < expressao.size(); i++) {
                if (expressao[i] == 118 || expressao[i] == 94 || expressao[i] == 62) {
                    isResolvido = false;
                    break;
                }

            }


        }
        std::wcout <<  " ------- " << std::endl;




       
    }


}



