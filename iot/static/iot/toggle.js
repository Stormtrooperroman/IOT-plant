$(".Toggle").click(function() {
    if( $(this).attr('aria-pressed') === ('true')) {
        $(this).removeAttr('aria-pressed');
    } else {
        $(this).attr('aria-pressed', 'true');
    }
});
