function printw() {
    $("#imgprint").hide();
    window.print();

}
window.onafterprint = function () {
    $("#imgprint").show(); window.close();
}
$(document).ready(function () {
    printw();
})
jQuery(document).bind("keyup keydown", function (e) {
    if (e.ctrlKey && e.keyCode == 80) {
        $("#imgprint").hide();
    }
});