let minimum = (a, b, c) => min(a, min(b, c)) /* * Levenshtein distance * Translated from OCaml example in Rosetta Code * https://rosettacode.org/wiki/Levenshtein_distance#OCaml * https://en.wikipedia.org/wiki/Levenshtein_distance */;

let distance = (s, t) => {
  let first = String.length(s)
  and second = String.length(t);
  let matrix = Array.make_matrix(first + 1, second + 1, 0);

  for (i in 0 to first) {
    matrix[i][0] = i;
  };

  for (j in 0 to second) {
    matrix[0][j] = j;
  };

  for (j in 1 to second) {
    for (i in 1 to first) {
      if (s.[i - 1] == t.[j - 1]) {
        matrix[i][j] = matrix[i - 1][j - 1];
      } else {
        matrix[i][j] =
          minimum(
            matrix[i - 1][j] + 1,
            matrix[i][j - 1] + 1,
            matrix[i - 1][j - 1] + 1,
          );
      };
    };
  };

  matrix[first][second];
};
