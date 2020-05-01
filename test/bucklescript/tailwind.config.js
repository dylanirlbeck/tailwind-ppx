module.exports = {
  important: true,
  theme: {},
  extend: {},
  variants: {
    display: ["responsive", "hover", "focus", "group-hover"],
    boxShadow: ["focus", "active"],
    backgroundColor: ["hover", "focus", "active", "focus-within"],
    borderColor: ["hover", "focus", "active", "focus-within"],
    text: ["hover", "focus", "active", "group-hover", "focus-within"],
    margin: ["last"],
    borderRadius: ["last", "first"],
    borderWidth: ["last", "first", "odd", "even"]
  }
};
