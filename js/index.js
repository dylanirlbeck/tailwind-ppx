function extractor(content) {
  const matchedContent = content.match(/(?<=\%tw(\s*|\()")[^\"]*(?=\")/g);

  if (matchedContent) {
    return matchedContent.reduce((acc, curr) => {
      curr.split(" ").forEach(p => {
        const trimmed = p.trim();

        if (trimmed !== "") {
          acc.push(trimmed);
        }
      });

      return acc;
    }, []);
  }

  return [];
}

module.exports = { extractor };
