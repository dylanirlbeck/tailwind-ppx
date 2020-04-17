let acceptableClasses = ["mt-2", "flex", "flex-row"];

let splitClassNames = classNames => {
  String.split_on_char(' ', classNames);
};

let rec validateClassNames = (splitClassNames, loc) => {
  let validateClassName = className => {
    List.exists(
      acceptableClassName => acceptableClassName == className,
      acceptableClasses,
    );
  };
  switch (splitClassNames) {
  | [] => ()
  | [className, ...remainingClassNames] =>
    if (validateClassName(className)) {
      validateClassNames(remainingClassNames, loc);
    } else {
      raise(
        Location.Error(
          Location.error(~loc, "Class name not found: " ++ className),
        ),
      );
    }
  };
};

let validate = (classNames, loc) => {
  let splitClasses = splitClassNames(classNames);
  validateClassNames(splitClasses, loc);
};
