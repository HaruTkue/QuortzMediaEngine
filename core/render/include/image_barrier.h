struct ImageLayoutTransition{
    VkImageLayout oldLayout;
    VkImageLayout newLayout;
    VkAccessFlags srcAccessMask;
    VkAccessFlags dstAccessMask;
    VkPipelineStageFlags srcStage;
    VkPipelineStageFlags dstStage;


    //undefine --> draw
    static ImageLayoutTransition FromUndefinedToColorAttachment();
    //presentsrc --> drawLayout
    static ImageLayoutTransition FromPresentSrcToColorAttachment();
    //draw --> present Src
    static ImageLayoutTransition FromColorToPresent();
};