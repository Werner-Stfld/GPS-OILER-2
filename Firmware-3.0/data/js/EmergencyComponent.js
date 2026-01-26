import LabelledInput from "./LabelledInputComponent.js"
export default {
  setup() {

    const {inject, onMounted} = Vue;

    const settings = inject('emergency')

    function apply () {
      console.log("apply emergency settings: " + JSON.stringify(settings))
    }
    return {
      apply, settings
    }
  },
// {
//     "timeout": 2,
//     "speed": 60
// }
  template: 
  `<div> \
    <form @submit.prevent="apply"> \
      <fieldset class="grid-form label-box">\
        <legend>Emergency</legend>\
        <labelled-input summary="Timeout" desc="Time in seconds to enable emergency mode, after GPS speed cannot be determined. Min: 30 sec"/>\
        <input id="Timeout" type="number" min="30" max="240" v-model="settings.timeout" name="timeout"/>\
        sec\
        <labelled-input summary="Speed" desc="Assumed speed while emergency mode is active. 0: -> disabled emergency mode."/>\
        <input id="Speed" type="number" min="0" max="200" v-model="settings.speed" name="speed"/>\
        km/h\
      </fieldset>\
    </form>\
  </div>`,
  components: {
    LabelledInput
  }
}