export default {
  props: {
      summary: {
        type: String,
        required: true
      },
      desc: {
        type: String,
        required: true
      }
  },
  template: `<div > \
              <label for="{{ summary }}" > \
                <details> \
                  <summary>{{ summary }}</summary> \
                  <p>{{ desc }}</p> \
                </details> \
              </label> \
            </div>`
}