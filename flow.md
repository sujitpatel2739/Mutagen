User Broken Code
       │
       ▼
   Lexer → Parser → AST
       │
       ├── Syntax OK → feed directly to GA
       └── Syntax Not OK → GA starts with broken AST
              │
              ▼
        GAController
       /     |      \
 Mutation  Crossover  Selection
       \     |      /
        Fitness Evaluator
              │
              ▼
        Compiles? Runs?
        Passes tests?
              │
           evolves
              │
              ▼
       Final Working Code
