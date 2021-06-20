#include "breezetextbutton.h"
#include "breezedecoration.h"

#include <KColorUtils>

#include <QPainter>

using namespace KDecoration2;
namespace Breeze
{

using KDecoration2::ColorRole;
using KDecoration2::ColorGroup;
using KDecoration2::DecorationButtonType;

TextButton::TextButton(const QPointer<KDecoration2::Decoration> &decoration,int row, QObject *parent) :
    DecorationButton(DecorationButtonType::Custom, decoration, parent)
  , m_animation( new QVariantAnimation( this ) )
  , m_buttonIndex(row)
{
    setEnabled(true);

    m_animation->setStartValue( 0.0 );
    m_animation->setEndValue( 1.0 );
    m_animation->setEasingCurve( QEasingCurve::InOutQuad );
    m_animation->stop();


    connect(m_animation, &QVariantAnimation::valueChanged, this, [this](const QVariant &value) {

        setOpacity(value.toReal());
    });
    auto d = qobject_cast<Decoration*>(decoration);

    connect( this, &KDecoration2::DecorationButton::hoveredChanged, this, &TextButton::updateAnimationState );

    connect(this, &TextButton::clicked, this, [this,d](){
        if(!m_action) {
            return;
        }

        auto menu = m_action->menu();

        auto pt = d->windowPos()+QPoint(geometry().x(),- Metrics::TitleBar_BottomMargin*d->settings()->smallSpacing());

        auto geom = menu->geometry();

        geom.setTopLeft(pt);

        menu->setGeometry(geom);
        menu->adjustSize();
        menu->show();
    });

    // animation
    if( d )  m_animation->setDuration( d->animationsDuration() );
}

QAction *TextButton::action() const
{
    return m_action;
}

void TextButton::setAction(QAction *newAction)
{
    m_action = newAction;
}

const QString &TextButton::text() const
{
    return m_text;
}

void TextButton::setText(const QString &newText)
{
    auto deco = qobject_cast<Decoration*>(decoration());
    if (m_text == newText)
        return;
    m_text = newText;

    const QSizeF textSize = {deco->textWidth(newText), static_cast<double>(deco->captionHeight())};

    auto geom = geometry();
    geom.setSize({textSize.width(), textSize.height()});
    setGeometry(geom);


    emit textChanged();

}

void TextButton::paint(QPainter *painter, const QRect &repaintArea)
{
    auto d = qobject_cast<Decoration*>( decoration() );

    Q_UNUSED(repaintArea)

    if (!decoration()) return;

    painter->save();
    painter->setPen(Qt::transparent);

    auto textRect = painter->fontMetrics().boundingRect(m_text);
    textRect.moveCenter(geometry().translated(Metrics::TitleBar_SideMargin*d->settings()->smallSpacing(),0).center().toPoint());


    if (isPressed()){
           painter->setBrush(KColorUtils::mix( d->titleBarColor(), d->fontColor(), 0.3 ));
           painter->drawRoundedRect(geometry(), 3,3);
           painter->setPen(d->titleBarColor());
           painter->drawText(textRect,
                             Qt::TextSingleLine | Qt::TextShowMnemonic,  m_text);

    } else if(isHovered() ||(m_action && m_action->menu() && m_action->menu()->isVisible())){
        painter->setBrush(d->fontColor());
        painter->drawRoundedRect(geometry(), 3,3);
        painter->setPen(d->titleBarColor());
        painter->drawText(textRect,
                          Qt::TextSingleLine | Qt::TextShowMnemonic,  m_text);


    } else {

        painter->setPen(d->fontColor());
        painter->drawText(textRect,
                          Qt::TextSingleLine | Qt::TextShowMnemonic,  m_text);
    }



    painter->restore();
}
void TextButton::updateAnimationState( bool hovered )
{
    auto d = qobject_cast<Decoration*>(decoration());
    if( !(d && d->animationsDuration() > 0 ) ) return;

    m_animation->setDirection( hovered ? QAbstractAnimation::Forward : QAbstractAnimation::Backward );
    if( m_animation->state() != QAbstractAnimation::Running ) m_animation->start();

}

}
