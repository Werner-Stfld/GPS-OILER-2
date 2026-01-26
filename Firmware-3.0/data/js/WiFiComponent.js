import LabelledInput from "./LabelledInputComponent.js"
export default {
  setup() {

    const {inject, onMounted} = Vue;

    const settings = inject('wifi')

    function apply () {
      console.log("apply wifi settings: " + JSON.stringify(settings))
    }
    return {
      apply, settings
    }
  },
  template: 
  `<div> \
    <form @submit.prevent="apply"> \
      <fieldset class="grid-form label-box">\
        <legend>WiFi</legend>\
        <labelled-input summary="SSID" desc="Name of the WiFi network, default GPS-OILER"/>\
        <input id="SSID" type="text" maxlength="20" v-model="settings.name" name="name"/>\
        &nbsp;\
        <labelled-input summary="Password" desc="Password to secure the WiFi access point. The password should be at least 8 chars long. An empty password allows unrestricted access."/>\
        <input id="Password" type="text" maxlength="20" v-model="settings.password" name="password" />\
        &nbsp;\
      </fieldset>\
    </form>\
  </div>`,
  components: {
    LabelledInput
  }
}