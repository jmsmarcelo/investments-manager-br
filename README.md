# Gerenciador de Investimentos

O **Gerenciador de Investimentos** é uma ferramenta para investidores que desejam gerenciar seus portfólios nas bolsas brasileira (B3) e americanas (principalmente NYSE e NASDAQ), com foco na conformidade fiscal brasileira. Todas as informações de transações e ativos são inseridas manualmente, enquanto cálculos, relatórios, conversões cambiais e despesas operacionais (após configuração) são gerados automaticamente.

## ✨ Funcionalidades

- **Rastreamento de Ativos**: Registre manualmente ações, ETFs, FIIs e outros ativos negociados na B3, NYSE, NASDAQ ou outras bolsas, informando o nome da bolsa.
- **Dividendos e JCP**: Acompanhe ganhos de dividendos e Juros sobre Capital Próprio (JCP), com entrada manual de valores, datas e bolsa associada.
- **Despesas Operacionais**: Configure taxas de corretagem, bolsa e tributos na seção de configurações de despesas. Com base nessas configurações, o sistema gera automaticamente despesas operacionais associadas às transações.
- **Conversão Cambial Automática**: Converta valores em USD para BRL no frontend usando a API gratuita do Banco Central (PTAX), com base na data da operação, para conformidade com a Receita Federal.
- **Cálculo Automático de Ganhos e Perdas**: Compute automaticamente lucros e prejuízos com base nas transações e despesas (incluindo as geradas automaticamente), considerando preços de compra/venda e custos operacionais em BRL.
- **Tributação Brasileira**: Gere relatórios automáticos de Imposto de Renda (IRPF) e DARF, usando valores convertidos para BRL conforme as regras da Receita Federal.
- **Interface Simples**: Desenvolvido com HTML, CSS e Vanilla JS para uma experiência leve e intuitiva.
- **Backend Robusto**: Implementado em C para alta performance, com PostgreSQL para armazenamento seguro e eficiente dos dados.
  
⚠️ **Nota**: Todas as informações de transações, dividendos, JCP e ativos são inseridas manualmente. Despesas operacionais (corretagem, bolsa, tributos) são geradas automaticamente após configuração na seção de despesas. O sistema suporta principalmente B3, NYSE e NASDAQ, mas permite registrar ativos e despesas de outras bolsas via entrada manual. A conversão cambial é feita automaticamente no frontend usando a API do Banco Central.
