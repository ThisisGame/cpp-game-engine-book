$( document ).ready(function() {

    $("div.headertitle").addClass("page-header");
    $("div.title").addClass("h1");
    
    // some ugly hacks because of doxygen bugs
    var title = $("title");
    if (title.text() == "rttr: Main Page")
    {
      title.text("RTTR - Run Time Type Reflection");
      var header_title = $("div.title.h1");
      if (header_title.text() == "rttr Documentation")
        header_title.text("RTTR - Run Time Type Reflection");
    }
    else 
    {
        var new_title = title.text().replace("rttr:", "RTTR:");
        title.text(new_title);
    }

    $('li > a[href="index.html"] > span').before("<i class='fa fa-cog'></i> ");
    var tutorial_link = $('li > a[href="pages.html"]');
    tutorial_link.attr("href", "tutorial_page.html");
    
    var install_link = $('li > a > span:contains("Installation")').parent();
    install_link.attr("href", "building_install_page.html");
    
    var license_link = $('li > a > span:contains("License")').parent();
    license_link.attr("href", "license_page.html");
    
    if (title.text().search("Building & Installation") > - 1)
    {
        tutorial_link.parent().removeClass("current active");
        install_link.parent().addClass("current active");
    }
    else if (title.text().search("License") > -1)
    {
        tutorial_link.parent().removeClass("current active");
        license_link.parent().addClass("current active");
    }
    
    // replace the div tags with the <a> tags below,
    // the reason we are doing this, is because we cannot style directly the <a> tags with classes within doxygen
    // and explicit <a> tags cannot be using with internal links
    var button_link = $("div.doxy-button").each(function(){
        var a_tag = $(this).children().first();
        if (a_tag.length == 0)
            return;
        a_tag.addClass($(this).attr("class"));
        $(this).replaceWith(a_tag);
    });
    
    $('li > a[href="modules.html"] > span').before("<i class='fa fa-square'></i> ");
    $('li > a[href="namespaces.html"] > span').before("<i class='fa fa-bars'></i> ");
    $('li > a[href="annotated.html"] > span').before("<i class='fa fa-list-ul'></i> ");
    $('li > a[href="classes.html"] > span').before("<i class='fa fa-book'></i> ");
    $('li > a[href="inherits.html"] > span').before("<i class='fa fa-sitemap'></i> ");
    $('li > a[href="functions.html"] > span').before("<i class='fa fa-list'></i> ");
    $('li > a[href="functions_func.html"] > span').before("<i class='fa fa-list'></i> ");
    $('li > a[href="functions_vars.html"] > span').before("<i class='fa fa-list'></i> ");
    $('li > a[href="functions_enum.html"] > span').before("<i class='fa fa-list'></i> ");
    $('li > a[href="functions_eval.html"] > span').before("<i class='fa fa-list'></i> ");
    $('img[src="ftv2ns.png"]').replaceWith('<span class="label label-danger">N</span> ');
    $('img[src="ftv2cl.png"]').replaceWith('<span class="label label-danger">C</span> ');

    $("ul.tablist").addClass("nav nav-pills nav-justified");
    
    $("ul.tablist").css("margin-top", "0.5em");
    $("ul.tablist").css("margin-bottom", "0.5em");
    $("li.current").addClass("active");
    $("iframe").attr("scrolling", "yes");

    $("#nav-path > ul").addClass("breadcrumb");

    $("table.params").addClass("table");
    $("div.ingroups").wrapInner("<small></small>");
    $("div.levels").css("margin", "0.5em");
    $("div.levels > span").addClass("btn btn-default btn-xs");
    $("div.levels > span").css("margin-right", "0.25em");

    $("table.directory").addClass("table table-striped");
    $("div.summary > a").addClass("btn btn-default btn-xs");
    $("table.fieldtable").addClass("table");
    $(".fragment").addClass("well");
    $(".memitem").addClass("panel panel-default");
    $(".memproto").addClass("panel-heading");
    $(".memdoc").addClass("panel-body");
    $("span.mlabel").addClass("label label-info");

    $("table.memberdecls").addClass("table");
    $("[class^=memitem]").addClass("active");

    $("div.ah").addClass("btn btn-default");
    $("span.mlabels").addClass("pull-right");
    $("table.mlabels").css("width", "100%")
    $("td.mlabels-right").addClass("pull-right");

    $("div.ttc").addClass("panel panel-primary");
    $("div.ttname").addClass("panel-heading");
    $("div.ttname a").css("color", 'white');
    $("div.ttdef,div.ttdoc,div.ttdeci").addClass("panel-body");

    $('#MSearchBox').parent().remove();

    $('div.fragment.well div.line:first').css('margin-top', '15px');
    $('div.fragment.well div.line:last').css('margin-bottom', '15px');

    $('table.doxtable').removeClass('doxtable').addClass('table table-striped table-bordered').each(function(){
        $(this).prepend('<thead></thead>');
        $(this).find('tbody > tr:first').prependTo($(this).find('thead'));
        
        $(this).find('td > span.success').parent().addClass('success');
        $(this).find('td > span.warning').parent().addClass('warning');
        $(this).find('td > span.danger').parent().addClass('danger');
    });
    

    if($('div.fragment.well div.ttc').length > 0)
    {
        $('div.fragment.well div.line:first').parent().removeClass('fragment well');
    }
    
/*
    $('table.memberdecls').find('.memItemRight').each(function(){
        $(this).contents().appendTo($(this).siblings('.memItemLeft'));
        $(this).siblings('.memItemLeft').attr('align', 'left');
    });
    */

    $('.mdescLeft').each(function(){
      $(this).parent().remove();
    });
    
    $('.memSeparator').each(function(){
      $(this).parent().remove();
    });
    
     $('.memTemplParams').each(function(){
      $(this).parent().remove();
    });
    
    
    function getOriginalWidthOfImg(img_element) {
        var t = new Image();
        t.src = (img_element.getAttribute ? img_element.getAttribute("src") : false) || img_element.src;
        return t.width;
    }
    
    $('div.dyncontent').find('img').each(function(){
        if(getOriginalWidthOfImg($(this)[0]) > $('#content>div.container').width())
            $(this).css('width', '100%');
    });
    
    $(".memitem").removeClass('memitem');
    $(".memproto").removeClass('memproto');
    $(".memdoc").removeClass('memdoc');
    $("span.mlabel").removeClass('mlabel');
    $("table.memberdecls").removeClass('memberdecls');
    $("[class^=memitem]").removeClass('memitem');
    $("span.mlabels").removeClass('mlabels');
    $("table.mlabels").removeClass('mlabels');
    $("td.mlabels-right").removeClass('mlabels-right');
    $(".navpath").removeClass('navpath');
    $("li.navelem").removeClass('navelem');
    $("a.el").removeClass('el');
    $("div.ah").removeClass('ah');
    $("div.header").removeClass("header");
    /*
    $('.mdescLeft').each(function(){
        if($(this).html()=="&nbsp;") {
            $(this).siblings('.mdescRight').attr('colspan', 2);
            $(this).remove();
        }
    });
    $('td.memItemLeft').each(function(){
        if($(this).siblings('.memItemRight').html()=="") {
            $(this).attr('colspan', 2);
            $(this).siblings('.memItemRight').remove();
        }
    });
  */
});