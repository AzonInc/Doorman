const GITHUB_TOKEN = process.env.GITHUB_TOKEN;

export default {
  async load() {
    const owner = "AzonInc";
    const repo = "Doorman";
    const perPage = 100;

    let page = 1;
    let all = [];

    while (true) {
      const url = `https://api.github.com/repos/${owner}/${repo}/contributors?per_page=${perPage}&page=${page}`;
      const res = await fetch(url, {
        headers: {
          "Accept": "application/vnd.github+json",
          ...(GITHUB_TOKEN && { Authorization: `Bearer ${GITHUB_TOKEN}` }),
        },
      });

      if (!res.ok) {
        throw new Error(`GitHub API Error: ${res.status} ${res.statusText}`);
      }

      const data = await res.json();
      all = all.concat(data);

      if (data.length < perPage) break;
      page++;
    }

    const contributors = all
      .filter(c => c.type !== "Bot")
      .map(c => ({
        name: c.login,
        avatar: c.avatar_url,
        contributions: c.contributions,
        title: '',
        links: [
          { icon: 'github', link: c.html_url },
        ],
      }));

    return contributors;
  },
};
