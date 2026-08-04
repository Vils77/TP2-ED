# **Plano de Gerenciamento de Configuração de Software (PGCS)**

**Projeto: Sistema de Catalogação Bibliográfica do NDH (Núcleo de Documentação Histórica \- UFMS/CPTL)**

**Histórico de Alteração do Documento**

| Data | Versão | Descrição | Autor |
| ----- | ----- | ----- | ----- |
| 03/08/26 | 1.0 | Criação da primeira versão do documento | Victor Elias |
| 04/08/26 | 1.1 | Revisão geral: inclusão de gestão de segredos, ambientes e releases, proteção de dados pessoais (LGPD); correção de ambiguidade na estratégia de branches; refinamento dos quality gates. | Victor Elias |
|  |  |  |  |

## **1\. Introdução e Objetivos**

Este Plano de Gerenciamento de Configuração de Software (PGCS) estabelece as diretrizes, padrões e ferramentas para controlar, versionar e auditar os artefatos do sistema de gerenciamento da biblioteca setorial do Núcleo de Documentação Histórica (NDH) da UFMS/CPTL (cerca de 2.000 volumes).

O objetivo principal é garantir a rastreabilidade das alterações, a integridade do código e do banco de dados, e a execução contínua de testes automatizados de qualidade antes do envio para produção.

## **2\. Itens de Configuração (CI \- Configuration Items)**

Serão mantidos sob rigoroso controle de versão os seguintes elementos do projeto:

* **Código-fonte da Aplicação:** Backend (PHP / CakePHP), Microsserviços/APIs (Node.js), Frontend Web (Vue.js).  
* **Configuração de Autenticação & Autorização:** Configurações de Realms, Clients e Roles do Keycloak (Exportação de configurações em JSON/YAML).  
* **Estrutura de Banco de Dados:** Migrations nativas do CakePHP (Phinx) e/ou ORM do Node.js.  
* **Gerenciamento de Dependências:** Arquivos de trava composer.lock (PHP), package-lock.json ou yarn.lock (Node.js).  
* **Pipeline de CI/CD e Containerização:** Arquivos docker-compose.yml, Dockerfiles e scripts do GitHub Actions / GitLab CI.  
* **Documentação Técnica e Especificações:** Diagramas de arquitetura, documento de requisitos, documento de caso de teste, relatorio de bug e OpenAPI/Swagger.

* **Suíte de Testes Automatizados:** Testes unitários, de integração e testes de interface E2E (PHPUnit, Vitest/Jest, e Playwright).

* **Configuração de Ambiente:** Modelos de variáveis de ambiente (.env.example) por serviço, sem valores reais de segredo — ver Seção 9\.

* **Infraestrutura como Código:** Manifestos de provisionamento e configuração de ambientes (Docker Compose, scripts de deploy).

## **3\. Estratégia de Versionamento e Ramificação (Git Workflow)**

O projeto utilizará o modelo **Gitflow Simplificado** para garantir a organização das branches e impedir código instável no ambiente principal.

| Nome da Branch | Descrição e Finalidade | Regras de Proteção   |
| :---: | :---: | :---: |
| **main** | Branch de produção. Contém o código estável e pronto para implantação no NDH. | Merge bloqueado sem Pull Request aprovado e pipeline de CI/CD 100% verde. |
| **develop** | Branch de integração de funcionalidades em desenvolvimento. | Exige execução de testes automatizados com sucesso. |
| **feature/\<nome\>** | Branches temporárias para criação de novas funcionalidades (ex: feature/login-keycloak). | Criada a partir da develop. Removida após o merge. |
| **hotfix/\<nome\>** | Correções urgentes em produção (ex: hotfix/erro-emprestimo). | Criada a partir da main e integrada à main e develop. |
| **docs/\<numero da issue\>/nome-da-branch** | Criação e atualizações de documentos do projeto | Criada a partir da main e usada exclusivamente para fins de documentação |

## 

## **4\. Padrões de Commit e Mensagens de Alteração**

Todas as mensagens de commit devem seguir o padrão **Conventional Commits** para manter o histórico auditável:

feat(keycloak): integra autenticacao SSO via OpenID Connect no VueJS  
fix(cakephp): ajusta migraçao da tabela de empréstimos e regras ABNT  
test(cakephp): adiciona teste para busca remota de acervo  
chore(node): atualiza dependencias no package.json e lockfile

## **5\. Integração Contínua (CI) e Portões de Qualidade (Quality Gates)**

A automação via CI/CD executará verificações específicas para a stack selecionada a cada Pull Request:

* **Restauro de Dependências:** Validação de integridade via composer install, npm ci.  
* **Análise Estática e Linter:**  
  * PHP CodeSniffer / PHPStan para CakePHP.  
  * ESLint e Prettier para Node.js e Vue.js.  
* **Testes Unitários e de Integração:**  
  * **PHPUnit:** Validação das regras de negócio do CakePHP (tombo automático, ABNT, cálculo de devolução).  
  * **Vitest / Jest:** Testes das rotas Node.js e componentes Vue.js.  
* **Testes End-to-End (E2E):** Execução de testes no Playwright para validar a navegação completa da web (Vue.js \+ Keycloak \+ Backend).

* **Cobertura mínima de testes:** Pull Requests que reduzam a cobertura de testes abaixo de 70% no backend PHP e 60% nas APIs Node.js/componentes Vue.js são bloqueados automaticamente.

Se qualquer uma das etapas falhar, o merge será automaticamente bloqueado.

## **6\. Gerenciamento de Mudanças no Banco de Dados e Autenticação (Keycloak)**

É proibido alterar esquemas de banco de dados e perfis de acesso diretamente em produção. Mudanças devem ser automatizadas:

* **Migrations de Banco de Dados:** Utilização do plugin de Migrations do CakePHP (Phinx) e/ou ferramentas de migração no Node.js. Arquivos versionados em repositório (ex: 20260803120000\_CreateObras.php).  
* **Configurações do Keycloak:** As configurações do Realm do Keycloak (Roles de administradores e usuários) devem ser exportadas em arquivos JSON/YAML e armazenadas no repositório no diretório /config/keycloak/.

* **Plano de rollback:** toda migration destrutiva (que altere ou remova colunas/tabelas existentes) deve ter um script de reversão testado antes do merge. Em caso de falha em produção, o procedimento padrão é: (1) reverter o deploy para a última tag estável, (2) executar o rollback da migration correspondente, (3) restaurar backup apenas se os passos anteriores não restabelecerem a integridade dos dados.

## **7\. Matriz de Ferramentas por Camada da Stack**

| Camada / Módulo | Tecnologia Selecionada | Ferramentas de Teste e GCS Associadas   |
| :---: | :---: | :---: |
| **Backend Principal** | PHP / CakePHP | Composer (GCS), PHPUnit (Testes), PHPStan / PHP\_CodeSniffer (Qualidade), CakePHP Migrations. |
| **APIs / Servidores Auxiliares** | Node.js | npm / yarn (GCS), Vitest / Jest (Testes Unitários/API), ESLint (Linter). |
| **Frontend Web** | Vue.js | Vue Test Utils / Vitest (Testes de Componentes), Playwright (Testes E2E Web). |
| **Autenticação & Autorização** | Keycloak | Keycloak Realm Export/Import (JSON/YAML no Git), Docker para instância de testes de integração. |
| **CI/CD & Controle de Versão** | Git \+ GitHub Actions / GitLab CI | Orquestração de testes automatizados, verificação de linters e travas de branch. |

## **8\. Gestão de Defeitos: Matriz de Severidade e Prioridade**

Para garantir um processo estruturado de Garantia da Qualidade (QA), os defeitos identificados durante a execução dos testes automatizados ou testes manuais de homologação deverão ser categorizados por **Severidade** (impacto técnico) e **Prioridade** (urgência de negócio).

### **8.1 Definição de Severidade (Impacto no Sistema)**

* **S1 \- Bloqueante (CRITICAL):** O sistema fica indisponível ou uma funcionalidade crítica é totalmente interrompida sem solução alternativa (ex: Keycloak indisponível impedindo login; falha na geração do tombo automático bloqueando novos cadastros).  
* **S2 \- Alta (HIGH):** Impacto severo em funcionalidade principal, mas existe um contorno temporário (ex: falha na busca automática ABNT via API do Google Books, exigindo preenchimento manual).  
* **S3 \- Média (MEDIUM):** Impacto moderado em funcionalidade secundária ou degradação de desempenho sem perda de dados (ex: lentidão no filtro de doadores do acervo).  
* **S4 \- Baixa (LOW):** Problemas cosméticos, erros de digitação na interface ou pequenos desalinhamentos visuais (ex: erro de grafia em um rótulo na tela do app Flutter).

### **8.2 Definição de Prioridade (Atendimento pelo Desenvolvedor)**

* **P1 \- Urgente (IMMEDIATE):** Deve ser corrigido imediatamente na sprint/ciclo atual. Interrompe outras tarefas se estiver em ambiente de produção.  
* **P2 \- Alta (HIGH):** Deve ser corrigido no ciclo atual ou na próxima release planejada.  
* **P3 \- Média (MEDIUM):** Corrigido durante o fluxo normal de desenvolvimento após a resolução dos itens de maior prioridade.  
* **P4 \- Baixa (LOW):** Correção opcional ou agendada para refatorações futuras quando houver disponibilidade.

### **8.3 Matriz de Decisão e SLA de Resolução (Portão de QA)**

| Severidade \\ Prioridade | P1 \- Urgente | P2 \- Alta | P3 \- Média | P4 \- Baixa   |
| :---: | :---: | :---: | :---: | :---: |
| **S1 \- Bloqueante** | **Ação Imediata (Hotfix)** Deploy bloqueado até a solução. | Tratar no topo do Backlog da Sprint. | Não aplicável | Não aplicável |
| **S2 \- Alta** | Corrigir antes da liberação do Release Candidate. | **Ação Prioritária** Planejado na Sprint atual. | Agendado na próxima Sprint. | Não aplicável |
| **S3 \- Média** | Não aplicável | Avaliada no refinamento da Sprint. | **Fluxo Normal** Alocado conforme prioridade de negócio. | Pode ser postergado. |
| **S4 \- Baixa** | Não aplicável | Não aplicável | Ajustado conforme conveniência técnica. | **Melhoria Futura** Backlog de dívida técnica. |

**Regra de Transição para Produção (Quality Gate):** O software só será aprovado para implantação na branch main do NDH se contiver **zero defeitos S1/P1 e S2/P2 em aberto**.

# 

# 

# **9\. Ambientes e Versionamento de Releases**

## **9.1 Ambientes**

* **Desenvolvimento:** ambiente local dos desenvolvedores, com dados fictícios/anonimizados.

* **Homologação/Staging:** ambiente que espelha produção, alimentado a partir da branch develop, usado para validação funcional pela equipe do NDH antes de qualquer lançamento.

* **Produção:** ambiente acessado pelo NDH e pelo público, alimentado exclusivamente a partir de tags geradas na branch main.

## **9.2 Versionamento Semântico**

Releases da aplicação devem seguir Versionamento Semântico (SemVer: MAJOR.MINOR.PATCH), com tags Git anotadas no momento do merge da branch release/\<versão\> para main (ex.: v1.2.0). O changelog de cada release deve listar as mudanças relevantes, derivado das mensagens de Conventional Commits (Seção 4).

# **10\. Proteção de Dados Pessoais (LGPD)**

O módulo de empréstimos coleta dados pessoais de usuários (nome, RGA, telefone/WhatsApp, e-mail). Para fins de gerenciamento de configuração, esses dados recebem tratamento diferenciado:

* Bancos de dados de homologação/desenvolvimento utilizam dados fictícios ou anonimizados — nunca cópia direta da base de produção.

* Alterações no esquema de dados que envolvam campos pessoais passam por aprovação antes do merge.

Este item trata apenas da dimensão de configuração/versionamento; a definição completa de tratamento de dados pessoais do sistema deve constar em documento específico de conformidade com a LGPD, fora do escopo deste PGCS.

# **11\. Revisão deste Documento**

Este PGCS deve ser revisado a cada marco relevante do projeto (mudança de stack, novo ambiente, incidente de perda de dados ou segurança) com o registro correspondente na tabela de Histórico de Alteração.

